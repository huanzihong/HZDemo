#include "MassZombieReplicator.h"

#include "FMassReplicationNavPathHandler.h"
#include "MassZombieBubble.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"

void UMassZombieReplicator::AddRequirements(FMassEntityQuery& EntityQuery)
{
	FMassReplicationProcessorPositionYawHandler::AddRequirements(EntityQuery);
	FMassReplicationProcessorNavPathHandler::AddRequirements(EntityQuery);
	EntityQuery.AddRequirement<FEnemyFragment>(EMassFragmentAccess::ReadOnly);
}

void UMassZombieReplicator::ProcessClientReplication(FMassExecutionContext& Context,
                                                     FMassReplicationContext& ReplicationContext)
{
#if UE_REPLICATION_COMPILE_SERVER_CODE

	FMassReplicationProcessorNavPathHandler NavPathHandler;
	FMassReplicationProcessorPositionYawHandler PositionYawHandler;
	FMassReplicationSharedFragment* RepSharedFrag = nullptr;
	TConstArrayView<FEnemyFragment> EnemyList;

	auto CacheViewsCallback = [&RepSharedFrag, &NavPathHandler, &PositionYawHandler, &EnemyList](FMassExecutionContext& Context)
	{
		NavPathHandler.CacheFragmentViews(Context);
		PositionYawHandler.CacheFragmentViews(Context);
		EnemyList = Context.GetFragmentView<FEnemyFragment>();
		RepSharedFrag = &Context.GetMutableSharedFragment<FMassReplicationSharedFragment>();
		check(RepSharedFrag);
	};

	auto AddEntityCallback = [&RepSharedFrag, &NavPathHandler, &PositionYawHandler, &EnemyList](FMassExecutionContext& Context, const int32 EntityIdx, FReplicatedZombieAgent& InReplicatedAgent, const FMassClientHandle ClientHandle)->FMassReplicatedAgentHandle
	{
		AMassZombieClientBubbleInfo& ZombieBubbleInfo = RepSharedFrag->GetTypedClientBubbleInfoChecked<AMassZombieClientBubbleInfo>(ClientHandle);

		NavPathHandler.AddEntity(EntityIdx, InReplicatedAgent.GetReplicatedNavPathDataMutable());
		PositionYawHandler.AddEntity(EntityIdx, InReplicatedAgent.GetReplicatedPositionYawDataMutable());
		InReplicatedAgent.GetReplicatedEnemyStateDataMutable().SetFromEnemyFragment(EnemyList[EntityIdx]);

		return ZombieBubbleInfo.GetZombieSerializer().Bubble.AddAgent(Context.GetEntity(EntityIdx), InReplicatedAgent);
	};

	auto ModifyEntityCallback = [&RepSharedFrag, &NavPathHandler, &EnemyList](FMassExecutionContext& Context, const int32 EntityIdx, const EMassLOD::Type LOD, const double Time, const FMassReplicatedAgentHandle Handle, const FMassClientHandle ClientHandle)
	{
		AMassZombieClientBubbleInfo& ZombieBubbleInfo = RepSharedFrag->GetTypedClientBubbleInfoChecked<AMassZombieClientBubbleInfo>(ClientHandle);

		FMassZombieClientBubbleHandler& Bubble = ZombieBubbleInfo.GetZombieSerializer().Bubble;

		const bool bLastClient = RepSharedFrag->CachedClientHandles.Last() == ClientHandle;
		(void)bLastClient;
		Bubble.GetNavPathHandlerMutable().SetBubbleMoveTarget(Handle, NavPathHandler.GetMoveTargetFragment(EntityIdx));
		Bubble.SetBubbleEnemyState(Handle, EnemyList[EntityIdx].EnemyState);

		// Don't call the PositionYawHandler here as we currently only replicate the position and yaw when we add an entity to Mass
	};

	auto RemoveEntityCallback = [&RepSharedFrag](FMassExecutionContext& Context, const FMassReplicatedAgentHandle Handle, const FMassClientHandle ClientHandle)
	{
		AMassZombieClientBubbleInfo& ZombieBubbleInfo = RepSharedFrag->GetTypedClientBubbleInfoChecked<AMassZombieClientBubbleInfo>(ClientHandle);

		ZombieBubbleInfo.GetZombieSerializer().Bubble.RemoveAgentChecked(Handle);
	};

	CalculateClientReplication<FZombieFastArrayItem>(Context, ReplicationContext, CacheViewsCallback, AddEntityCallback, ModifyEntityCallback, RemoveEntityCallback);
#endif // UE_REPLICATION_COMPILE_SERVER_CODE
}

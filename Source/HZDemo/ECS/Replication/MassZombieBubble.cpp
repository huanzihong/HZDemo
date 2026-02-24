#include "MassZombieBubble.h"

#include "Net/UnrealNetwork.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"

void FMassZombieClientBubbleHandler::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	auto AddRequirementsForSpawnQuery = [this](FMassEntityQuery& InQuery)
	{
		NavPathHandler.AddRequirementsForSpawnQuery(InQuery);
		TransformHandler.AddRequirementsForSpawnQuery(InQuery);
		InQuery.AddRequirement<FEnemyFragment>(EMassFragmentAccess::ReadWrite);
	};

	auto CacheFragmentViewsForSpawnQuery = [this](FMassExecutionContext& InExecContext)
	{
		NavPathHandler.CacheFragmentViewsForSpawnQuery(InExecContext);
		TransformHandler.CacheFragmentViewsForSpawnQuery(InExecContext);
	};

	auto SetSpawnedEntityData = [this](const FMassEntityView& EntityView, const FReplicatedZombieAgent& ReplicatedEntity, const int32 EntityIdx)
	{
		NavPathHandler.SetSpawnedEntityData(EntityIdx, ReplicatedEntity.GetReplicatedNavPathData());
		TransformHandler.SetSpawnedEntityData(EntityIdx, ReplicatedEntity.GetReplicatedPositionYawData());
		FEnemyFragment& EnemyFragment = EntityView.GetFragmentData<FEnemyFragment>();
		EnemyFragment.EnemyState = ReplicatedEntity.GetReplicatedEnemyStateData().GetEnemyState();
	};

	auto SetModifiedEntityData = [this](const FMassEntityView& EntityView, const FReplicatedZombieAgent& Item)
	{
		NavPathHandler.SetModifiedEntityData(EntityView, Item.GetReplicatedNavPathData());
		FEnemyFragment& EnemyFragment = EntityView.GetFragmentData<FEnemyFragment>();
		EnemyFragment.EnemyState = Item.GetReplicatedEnemyStateData().GetEnemyState();
	};

	PostReplicatedAddHelper(AddedIndices, AddRequirementsForSpawnQuery, CacheFragmentViewsForSpawnQuery, SetSpawnedEntityData, SetModifiedEntityData);
	
	NavPathHandler.ClearFragmentViewsForSpawnQuery();
	TransformHandler.ClearFragmentViewsForSpawnQuery();
}

void FMassZombieClientBubbleHandler::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	auto SetModifiedEntityData = [this](const FMassEntityView& EntityView, const FReplicatedZombieAgent& Item)
	{
		NavPathHandler.SetModifiedEntityData(EntityView, Item.GetReplicatedNavPathData());
		FEnemyFragment& EnemyFragment = EntityView.GetFragmentData<FEnemyFragment>();
		EnemyFragment.EnemyState = Item.GetReplicatedEnemyStateData().GetEnemyState();
	};

	PostReplicatedChangeHelper(ChangedIndices, SetModifiedEntityData);
}

#if UE_REPLICATION_COMPILE_SERVER_CODE
void FMassZombieClientBubbleHandler::SetBubbleEnemyState(const FMassReplicatedAgentHandle Handle, const EEnemyState InEnemyState)
{
	check(AgentHandleManager.IsValidHandle(Handle));

	const int32 AgentsIdx = AgentLookupArray[Handle.GetIndex()].AgentsIdx;
	FZombieFastArrayItem& Item = (*Agents)[AgentsIdx];

	FReplicatedEnemyStateData& ReplicatedState = Item.Agent.GetReplicatedEnemyStateDataMutable();
	if (ReplicatedState.UpdateFromEnemyState(InEnemyState))
	{
		Serializer->MarkItemDirty(Item);
	}
}
#endif // UE_REPLICATION_COMPILE_SERVER_CODE

/*void FMassZombieClientBubbleHandler::PostReplicatedChangeEntity(const FMassEntityView& EntityView,
	const FReplicatedZombieAgent& Item) const
{
}*/

AMassZombieClientBubbleInfo::AMassZombieClientBubbleInfo(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	Serializers.Add(&ZombieSerializer);
}

void AMassZombieClientBubbleInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	// Technically, this doesn't need to be PushModel based because it's a FastArray and they ignore it.
	DOREPLIFETIME_WITH_PARAMS_FAST(AMassZombieClientBubbleInfo, ZombieSerializer, SharedParams);
}

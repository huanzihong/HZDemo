#include "MassZombieReplicationRemovalProcessor.h"

#include "MassReplicationSubsystem.h"
#include "MassReplicationFragments.h"
#include "MassZombieBubble.h"

UMassZombieReplicationRemovalProcessor::UMassZombieReplicationRemovalProcessor()
	: EntityQuery(*this)
{
	ObservedType = FMassNetworkIDFragment::StaticStruct();
	ObservedOperations = EMassObservedOperationFlags::Remove;
	ExecutionFlags = int32(EProcessorExecutionFlags::Server);
	bRequiresGameThreadExecution = true;
}

void UMassZombieReplicationRemovalProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FMassNetworkIDFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<UMassReplicationSubsystem>(EMassFragmentAccess::ReadWrite);
}

void UMassZombieReplicationRemovalProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
#if UE_REPLICATION_COMPILE_SERVER_CODE
	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		UMassReplicationSubsystem& ReplicationSubsystem = Context.GetMutableSubsystemChecked<UMassReplicationSubsystem>();

		const FMassBubbleInfoClassHandle BubbleClassHandle =
			ReplicationSubsystem.GetBubbleInfoClassHandle(AMassZombieClientBubbleInfo::StaticClass());
		if (!BubbleClassHandle.IsValid())
		{
			return;
		}

		const TArray<FMassClientHandle>& ClientHandles = ReplicationSubsystem.GetClientReplicationHandles();
		const TConstArrayView<FMassNetworkIDFragment> NetIDList = Context.GetFragmentView<FMassNetworkIDFragment>();
		const int32 NumEntities = Context.GetNumEntities();

		for (int32 EntityIdx = 0; EntityIdx < NumEntities; ++EntityIdx)
		{
			const FMassNetworkID NetID = NetIDList[EntityIdx].NetID;
			if (!NetID.IsValid())
			{
				continue;
			}

			const FMassEntityHandle EntityHandle = Context.GetEntity(EntityIdx);

			for (const FMassClientHandle ClientHandle : ClientHandles)
			{
				if (!ReplicationSubsystem.IsValidClientHandle(ClientHandle))
				{
					continue;
				}

				AMassZombieClientBubbleInfo* BubbleInfo =
					ReplicationSubsystem.GetTypedClientBubble<AMassZombieClientBubbleInfo>(BubbleClassHandle, ClientHandle);
				if (BubbleInfo)
				{
					BubbleInfo->GetZombieSerializer().Bubble.RemoveAgent(NetID);
				}

				if (FMassClientReplicationInfo* ClientInfo = ReplicationSubsystem.GetMutableClientReplicationInfo(ClientHandle))
				{
					ClientInfo->AgentsData.Remove(EntityHandle);
				}
			}
		}
	});
#endif // UE_REPLICATION_COMPILE_SERVER_CODE
}

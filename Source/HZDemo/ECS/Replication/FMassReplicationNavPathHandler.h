#pragma once
#include "MassNavigationFragments.h"
#include "MassClientBubbleHandler.h"

#include "FMassReplicationNavPathHandler.generated.h"

USTRUCT()
struct FReplicatedAgentNavPathData
{
	GENERATED_BODY()

	FReplicatedAgentNavPathData(){};
	
	void SetMoveTargetCenter(const FVector& InPosition) { MoveTargetCenter = InPosition; }
	const FVector& GetMoveTargetCenter() const { return MoveTargetCenter; }
	
private:
	UPROPERTY(Transient)
	FVector MoveTargetCenter;
};

template<typename AgentArrayItem>
class TMassClientBubbleNavPathHandler
{
public:
	TMassClientBubbleNavPathHandler(TClientBubbleHandlerBase<AgentArrayItem>& InOwnerHandler)
		: OwnerHandler(InOwnerHandler)
	{}

#if UE_REPLICATION_COMPILE_SERVER_CODE
	/** Sets the move target data in the client bubble on the server */
	void SetBubbleMoveTarget(const FMassReplicatedAgentHandle Handle, const FVector& MoveTarget);

	// Another function  SetBubbleTransform() could be added here if required
#endif // UE_REPLICATION_COMPILE_SERVER_CODE

#if UE_REPLICATION_COMPILE_CLIENT_CODE
	/**
	 * When entities are spawned in Mass by the replication system on the client, a spawn query is used to set the data on the spawned entities.
	 * The following functions are used to configure the query and then set the position and yaw data.
	 */
	static void AddRequirementsForSpawnQuery(FMassEntityQuery& InQuery);
	void CacheFragmentViewsForSpawnQuery(FMassExecutionContext& InExecContext);
	void ClearFragmentViewsForSpawnQuery();

	void SetSpawnedEntityData(const int32 EntityIdx, const FReplicatedAgentNavPathData& ReplicatedPathData) const;

	/** Call this when an Entity that has already been spawned is modified on the client */
	static void SetModifiedEntityData(const FMassEntityView& EntityView, const FReplicatedAgentNavPathData& ReplicatedPathData);

	// We could easily add support replicating FReplicatedAgentTransformData here if required
#endif // UE_REPLICATION_COMPILE_CLIENT_CODE

protected:
#if UE_REPLICATION_COMPILE_CLIENT_CODE
	static void SetEntityData(FMassMoveTargetFragment& MoveTargetFragment, const FReplicatedAgentNavPathData& ReplicatedPathData);
#endif // UE_REPLICATION_COMPILE_CLIENT_CODE

protected:
	TArrayView<FMassMoveTargetFragment> MoveTargetList;

	TClientBubbleHandlerBase<AgentArrayItem>& OwnerHandler;
};

#if UE_REPLICATION_COMPILE_SERVER_CODE
template<typename AgentArrayItem>
void TMassClientBubbleNavPathHandler<AgentArrayItem>::SetBubbleMoveTarget(const FMassReplicatedAgentHandle Handle,  const FVector& MoveTarget)
{
	check(OwnerHandler.AgentHandleManager.IsValidHandle(Handle));

	const int32 AgentsIdx = OwnerHandler.AgentLookupArray[Handle.GetIndex()].AgentsIdx;
	bool bMarkDirty = false;

	AgentArrayItem& Item = (*OwnerHandler.Agents)[AgentsIdx];

	checkf(Item.Agent.GetNetID().IsValid(), TEXT("Pos should not be updated on FCrowdFastArrayItem's that have an Invalid ID! First Add the Agent!"));

	// GetReplicatedNavPathDataMutable() must be defined in your FReplicatedAgentBase derived class
	FReplicatedAgentNavPathData& ReplicatedNavPath = Item.Agent.GetReplicatedNavPathDataMutable();

	// Only update the Pos and mark the item as dirty if it has changed more than the tolerance
	const FVector Pos = MoveTarget;
	if (!Pos.Equals(ReplicatedNavPath.GetMoveTargetCenter(), 1.0))
	{
		ReplicatedNavPath.SetMoveTargetCenter(Pos);
		bMarkDirty = true;
	}

	if (bMarkDirty)
	{
		OwnerHandler.Serializer->MarkItemDirty(Item);
	}

}
#endif //UE_REPLICATION_COMPILE_SERVER_CODE

#if UE_REPLICATION_COMPILE_CLIENT_CODE
template<typename AgentArrayItem>
void TMassClientBubbleNavPathHandler<AgentArrayItem>::AddRequirementsForSpawnQuery(FMassEntityQuery& InQuery)
{
	InQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
}
#endif // UE_REPLICATION_COMPILE_CLIENT_CODE

#if UE_REPLICATION_COMPILE_CLIENT_CODE
template<typename AgentArrayItem>
void TMassClientBubbleNavPathHandler<AgentArrayItem>::CacheFragmentViewsForSpawnQuery(FMassExecutionContext& InExecContext)
{
	MoveTargetList = InExecContext.GetMutableFragmentView<FMassMoveTargetFragment>();
	
}
#endif // UE_REPLICATION_COMPILE_CLIENT_CODE

#if UE_REPLICATION_COMPILE_CLIENT_CODE
template<typename AgentArrayItem>
void TMassClientBubbleNavPathHandler<AgentArrayItem>::ClearFragmentViewsForSpawnQuery()
{
	MoveTargetList = TArrayView<FMassMoveTargetFragment>();
}
#endif // UE_REPLICATION_COMPILE_CLIENT_CODE

#if UE_REPLICATION_COMPILE_CLIENT_CODE
template<typename AgentArrayItem>
void TMassClientBubbleNavPathHandler<AgentArrayItem>::SetSpawnedEntityData(const int32 EntityIdx, const FReplicatedAgentNavPathData& ReplicatedPathData) const
{
	FMassMoveTargetFragment& MoveTargetFragment = MoveTargetList[EntityIdx];

	SetEntityData(MoveTargetFragment, ReplicatedPathData);
}
#endif // UE_REPLICATION_COMPILE_CLIENT_CODE

#if UE_REPLICATION_COMPILE_CLIENT_CODE
template<typename AgentArrayItem>
void TMassClientBubbleNavPathHandler<AgentArrayItem>::SetModifiedEntityData(const FMassEntityView& EntityView, const FReplicatedAgentNavPathData& ReplicatedPathData)
{
	FMassMoveTargetFragment& MoveTargetFragment = EntityView.GetFragmentData<FMassMoveTargetFragment>();

	SetEntityData(MoveTargetFragment, ReplicatedPathData);
}
#endif // UE_REPLICATION_COMPILE_CLIENT_CODE

#if UE_REPLICATION_COMPILE_CLIENT_CODE
template<typename AgentArrayItem>
void TMassClientBubbleNavPathHandler<AgentArrayItem>::SetEntityData(FMassMoveTargetFragment& MoveTargetFragment, const FReplicatedAgentNavPathData& ReplicatedPathData)
{
	MoveTargetFragment.Center = ReplicatedPathData.GetMoveTargetCenter();
}
#endif // UE_REPLICATION_COMPILE_CLIENT_CODE

class FMassReplicationProcessorNavPathHandler
{
public:
	/** Adds the requirements for the path following to the query. */
	static HZDEMO_API void AddRequirements(FMassEntityQuery& InQuery);

	/** Cache any component views you want to, this will get called before we iterate through entities. */
	HZDEMO_API void CacheFragmentViews(FMassExecutionContext& ExecContext);

	/**
	 * Set the replicated path data when we are adding an entity to the client bubble.
	 * @param EntityIdx the index of the entity in fragment views that have been cached.
	 * @param InOUtReplicatedPathData the data to set.
	 */
	HZDEMO_API void AddEntity(const int32 EntityIdx, FReplicatedAgentNavPathData& InOUtReplicatedPathData) const;
	HZDEMO_API FVector GetMoveTargetCenter(const int32 EntityIdx) const;

	/**
	 * Set the replicated path data when we are modifying an entity that already exists in the client bubble.
	 * @param Handle to the agent in the TMassClientBubbleHandler (that TMassClientBubblePathHandler is a member variable of).
	 * @param EntityIdx the index of the entity in fragment views that have been cached.
	 * @param BubblePathHandler handler to actually set the data in the client bubble
	 * @param bLastClient means it safe to reset any dirtiness
	 */
	/*template<typename AgentArrayItem>
	void ModifyEntity(const FMassReplicatedAgentHandle Handle, const int32 EntityIdx, TMassClientBubblePathHandler<AgentArrayItem>& BubblePathHandler, bool bLastClient);*/

	//TArrayView<FMassZoneGraphPathRequestFragment> PathRequestList;
	TArrayView<FMassMoveTargetFragment> MoveTargetList;
	//TArrayView<FMassZoneGraphLaneLocationFragment> LaneLocationList;
};


#pragma once
#include "MassNavigationFragments.h"
#include "MassClientBubbleHandler.h"
#include "Engine/World.h"

#include "FMassReplicationNavPathHandler.generated.h"

USTRUCT()
struct FReplicatedAgentNavPathData
{
	GENERATED_BODY()

	FReplicatedAgentNavPathData(){};

	void SetFromMoveTarget(const FMassMoveTargetFragment& MoveTargetFragment);
	bool UpdateFromMoveTarget(const FMassMoveTargetFragment& MoveTargetFragment);
	
	void SetMoveTargetCenter(const FVector& InPosition) { MoveTargetCenter = InPosition; }
	const FVector& GetMoveTargetCenter() const { return MoveTargetCenter; }

	void SetMoveTargetForward(const FVector& InForward) { MoveTargetForward = InForward; }
	const FVector& GetMoveTargetForward() const { return MoveTargetForward; }

	void SetDistanceToGoal(const float InDistance) { DistanceToGoal = InDistance; }
	float GetDistanceToGoal() const { return DistanceToGoal; }

	void SetSlackRadius(const float InSlackRadius) { SlackRadius = InSlackRadius; }
	float GetSlackRadius() const { return SlackRadius; }

	void SetDesiredSpeed(const FMassInt16Real& InDesiredSpeed) { DesiredSpeed = InDesiredSpeed; }
	const FMassInt16Real& GetDesiredSpeed() const { return DesiredSpeed; }

	void SetIntentAtGoal(const EMassMovementAction InIntentAtGoal) { IntentAtGoal = InIntentAtGoal; }
	EMassMovementAction GetIntentAtGoal() const { return IntentAtGoal; }

	void SetAction(const EMassMovementAction InAction) { Action = InAction; }
	EMassMovementAction GetAction() const { return Action; }

	void SetActionID(const uint16 InActionID) { ActionID = InActionID; }
	uint16 GetActionID() const { return ActionID; }

	void SetActionServerStartTime(const double InServerStartTime) { ActionServerStartTime = InServerStartTime; }
	double GetActionServerStartTime() const { return ActionServerStartTime; }

	void SetOffBoundaries(const bool bInOffBoundaries) { bOffBoundaries = bInOffBoundaries; }
	bool GetOffBoundaries() const { return bOffBoundaries; }

	void SetSteeringFallingBehind(const bool bInSteeringFallingBehind) { bSteeringFallingBehind = bInSteeringFallingBehind; }
	bool GetSteeringFallingBehind() const { return bSteeringFallingBehind; }
	
private:
	UPROPERTY(Transient)
	FVector MoveTargetCenter;
	UPROPERTY(Transient)
	FVector MoveTargetForward = FVector::ZeroVector;
	UPROPERTY(Transient)
	float DistanceToGoal = 0.0f;
	UPROPERTY(Transient)
	float SlackRadius = 0.0f;
	UPROPERTY(Transient)
	FMassInt16Real DesiredSpeed = FMassInt16Real(0.0f);
	UPROPERTY(Transient)
	EMassMovementAction IntentAtGoal = EMassMovementAction::Move;
	UPROPERTY(Transient)
	EMassMovementAction Action = EMassMovementAction::Move;
	UPROPERTY(Transient)
	uint16 ActionID = 0;
	UPROPERTY(Transient)
	double ActionServerStartTime = 0.0;
	UPROPERTY(Transient)
	bool bOffBoundaries = false;
	UPROPERTY(Transient)
	bool bSteeringFallingBehind = false;
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
	void SetBubbleMoveTarget(const FMassReplicatedAgentHandle Handle, const FMassMoveTargetFragment& MoveTargetFragment);

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
	void SetModifiedEntityData(const FMassEntityView& EntityView, const FReplicatedAgentNavPathData& ReplicatedPathData);

	// We could easily add support replicating FReplicatedAgentTransformData here if required
#endif // UE_REPLICATION_COMPILE_CLIENT_CODE

protected:
#if UE_REPLICATION_COMPILE_CLIENT_CODE
	void SetEntityData(FMassMoveTargetFragment& MoveTargetFragment, const FReplicatedAgentNavPathData& ReplicatedPathData) const;
#endif // UE_REPLICATION_COMPILE_CLIENT_CODE

protected:
	TArrayView<FMassMoveTargetFragment> MoveTargetList;

	TClientBubbleHandlerBase<AgentArrayItem>& OwnerHandler;
};

#if UE_REPLICATION_COMPILE_SERVER_CODE
template<typename AgentArrayItem>
void TMassClientBubbleNavPathHandler<AgentArrayItem>::SetBubbleMoveTarget(const FMassReplicatedAgentHandle Handle, const FMassMoveTargetFragment& MoveTargetFragment)
{
	check(OwnerHandler.AgentHandleManager.IsValidHandle(Handle));

	const int32 AgentsIdx = OwnerHandler.AgentLookupArray[Handle.GetIndex()].AgentsIdx;
	bool bMarkDirty = false;

	AgentArrayItem& Item = (*OwnerHandler.Agents)[AgentsIdx];

	checkf(Item.Agent.GetNetID().IsValid(), TEXT("Pos should not be updated on FCrowdFastArrayItem's that have an Invalid ID! First Add the Agent!"));

	// GetReplicatedNavPathDataMutable() must be defined in your FReplicatedAgentBase derived class
	FReplicatedAgentNavPathData& ReplicatedNavPath = Item.Agent.GetReplicatedNavPathDataMutable();

	bMarkDirty = ReplicatedNavPath.UpdateFromMoveTarget(MoveTargetFragment);

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
void TMassClientBubbleNavPathHandler<AgentArrayItem>::SetEntityData(FMassMoveTargetFragment& MoveTargetFragment, const FReplicatedAgentNavPathData& ReplicatedPathData) const
{
	MoveTargetFragment.Center = ReplicatedPathData.GetMoveTargetCenter();
	MoveTargetFragment.Forward = ReplicatedPathData.GetMoveTargetForward();
	MoveTargetFragment.DistanceToGoal = ReplicatedPathData.GetDistanceToGoal();
	MoveTargetFragment.SlackRadius = ReplicatedPathData.GetSlackRadius();
	MoveTargetFragment.DesiredSpeed = ReplicatedPathData.GetDesiredSpeed();
	MoveTargetFragment.IntentAtGoal = ReplicatedPathData.GetIntentAtGoal();
	MoveTargetFragment.bOffBoundaries = ReplicatedPathData.GetOffBoundaries();
	MoveTargetFragment.bSteeringFallingBehind = ReplicatedPathData.GetSteeringFallingBehind();

	UWorld* World = OwnerHandler.Serializer ? OwnerHandler.Serializer->GetWorld() : nullptr;
	if (World != nullptr)
	{
		if (MoveTargetFragment.GetCurrentActionID() != ReplicatedPathData.GetActionID())
		{
			MoveTargetFragment.CreateReplicatedAction(ReplicatedPathData.GetAction(), ReplicatedPathData.GetActionID(),
				World->GetTimeSeconds(), ReplicatedPathData.GetActionServerStartTime());
		}
	}
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
	HZDEMO_API const FMassMoveTargetFragment& GetMoveTargetFragment(const int32 EntityIdx) const;

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

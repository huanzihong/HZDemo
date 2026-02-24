#include "MassZombieBubble.h"

#include "Net/UnrealNetwork.h"

void FMassZombieClientBubbleHandler::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	auto AddRequirementsForSpawnQuery = [this](FMassEntityQuery& InQuery)
	{
		NavPathHandler.AddRequirementsForSpawnQuery(InQuery);
		TransformHandler.AddRequirementsForSpawnQuery(InQuery);
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
	};

	auto SetModifiedEntityData = [this](const FMassEntityView& EntityView, const FReplicatedZombieAgent& Item)
	{
		NavPathHandler.SetModifiedEntityData(EntityView, Item.GetReplicatedNavPathData());
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
	};

	PostReplicatedChangeHelper(ChangedIndices, SetModifiedEntityData);
}

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

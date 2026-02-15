#include "AIFindEnemyTask.h"

#include "MassCommonFragments.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

struct FTransformFragment;

EStateTreeRunStatus FAIFindEnemyTask::EnterState(FStateTreeExecutionContext& Context,
                                                 const FStateTreeTransitionResult& Transition) const
{
	const AActor& AIActor = Context.GetExternalData(ActorHandle);
	const UHashGridSubsystem& HashGridSubsystem = Context.GetExternalData(HashGridSubsystemHandle);
	const FEnemyHashGrid& EnemyHashGrid = HashGridSubsystem.GetHashGrid();
	const UMassEntitySubsystem& EntitySubsystem = Context.GetExternalData(EntitySubsystemHandle);
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const FVector CurrentLocation = AIActor.GetActorLocation();

	// 构建搜索范围
	FBox SearchBox = FBox::BuildAABB(CurrentLocation, FVector(InstanceData.SerchRadius));
	
	TArray<FMassEntityHandle> NearbyEnemy;
	EnemyHashGrid.Query(SearchBox, NearbyEnemy);

	if (NearbyEnemy.Num() == 0)
	{
		InstanceData.bFindEnemy = false;
		return EStateTreeRunStatus::Failed;
	}

	
	float ClosestDistanceSq = FLT_MAX;
	FMassEntityHandle ClosestEnemy;
	FVector ClosestEnemyLocation{};
	for (const FMassEntityHandle& EnemyEntity : NearbyEnemy)
	{
		if (!EntitySubsystem.GetEntityManager().IsEntityValid(EnemyEntity))
			continue;
		
		const FTransformFragment* EnemyTransform = EntitySubsystem.GetEntityManager().GetFragmentDataPtr<FTransformFragment>(EnemyEntity);
		if (!EnemyTransform)
			continue;

		const FVector EnemyLocation = EnemyTransform->GetTransform().GetLocation();
		const float DistanceSq = FVector::DistSquared(CurrentLocation, EnemyLocation);

		if (DistanceSq < ClosestDistanceSq)
		{
			ClosestEnemyLocation = EnemyLocation;
			ClosestDistanceSq = DistanceSq;
			ClosestEnemy = EnemyEntity;
		}
	}

	if (ClosestEnemy.IsValid())
	{
		InstanceData.bFindEnemy = true;
		InstanceData.EnemyLocation = ClosestEnemyLocation+FVector{0,0,90};
		return EStateTreeRunStatus::Running;
	}

	InstanceData.bFindEnemy = false;
	return EStateTreeRunStatus::Failed;
}

bool FAIFindEnemyTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(EntitySubsystemHandle);
	Linker.LinkExternalData(HashGridSubsystemHandle);
	Linker.LinkExternalData(ActorHandle);
	return FStateTreeAITaskBase::Link(Linker);
}

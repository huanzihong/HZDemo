#include "ZombieAnimProcessor.h"

#include "MassLODFragments.h"
#include "MassMovementFragments.h"
#include "MassRepresentationFragments.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"
#include "ECS/VetexAnim/Traits/ZombieAnimationFragment.h"

UZombieAnimInitializerProcessor::UZombieAnimInitializerProcessor():EntityQuery(*this)
{
	ObservedType = FZombieAnimationFragment::StaticStruct();
	Operation = EMassObservedOperation::Add;
}

void UZombieAnimInitializerProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FZombieAnimationFragment>(EMassFragmentAccess::ReadWrite);
}

void UZombieAnimInitializerProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
	{
		auto ZombieAnimationFragments = Context.GetMutableFragmentView<FZombieAnimationFragment>();
		
		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& ZombieAnimationFragment = ZombieAnimationFragments[EntityIdx];
			//加入一个偏移 这样每个僵尸走路时的动画就不会完全一样
			const float StartTimeOffset = FMath::FRandRange(0.0f, 10.0f);
			ZombieAnimationFragment.GlobalStartTime = StartTimeOffset;
		}
	});
}

UZombieAnimProcessor::UZombieAnimProcessor():AnimationEntityQuery(*this)
{
	ExecutionOrder.ExecuteAfter.Add(TEXT("UEnemyStateProcessor"));
	
}
void UZombieAnimProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	
	AnimationEntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
	
	AnimationEntityQuery.AddRequirement<FZombieAnimationFragment>(EMassFragmentAccess::ReadWrite);
	AnimationEntityQuery.AddRequirement<FMassRepresentationFragment>(EMassFragmentAccess::ReadOnly);
	AnimationEntityQuery.AddRequirement<FEnemyFragment>(EMassFragmentAccess::ReadOnly);
	//AnimationEntityQuery.AddRequirement<FMassRepresentationLODFragment>(EMassFragmentAccess::ReadOnly);
	//AnimationEntityQuery.AddChunkRequirement<FMassVisualizationChunkFragment>(EMassFragmentAccess::ReadOnly);
	//AnimationEntityQuery.SetChunkFilter(&FMassVisualizationChunkFragment::AreAnyEntitiesVisibleInChunk);
}

void UZombieAnimProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	AnimationEntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
	{
		TArrayView<FZombieAnimationFragment> AnimationDataList = Context.GetMutableFragmentView<FZombieAnimationFragment>();
		TConstArrayView<FMassVelocityFragment> VelocityList = Context.GetFragmentView<FMassVelocityFragment>();
		TConstArrayView<FMassRepresentationFragment> VisualizationList = Context.GetFragmentView<FMassRepresentationFragment>();
		TConstArrayView<FEnemyFragment> EnemyList = Context.GetFragmentView<FEnemyFragment>();
		
		for (FMassExecutionContext::FEntityIterator EntityIt = Context.CreateEntityIterator(); EntityIt; ++EntityIt)
		{
			FZombieAnimationFragment& AnimationData = AnimationDataList[EntityIt];
			const FEnemyFragment& Enemy = EnemyList[EntityIt];
			
			const FMassRepresentationFragment& Visualization = VisualizationList[EntityIt];
			const FMassVelocityFragment& Velocity = VelocityList[EntityIt];
			
			if (Visualization.CurrentRepresentation != EMassRepresentationType::None)
			{
				int32 StateIndex = 0;

				switch (Enemy.EnemyState)
				{
					case EEnemyState::ChasePlayer:
					    StateIndex = 0;
						break;
					case EEnemyState::Wander:
						StateIndex = 2;
						break;
					case EEnemyState::Knock:
						StateIndex = 4;
					break;
					
					default:
						break;
				}
				
				if (Velocity.Value.Size()<15 && StateIndex != 4)
				{
					StateIndex = 1;
				}
				
				AnimationData.PlayRate = 1.0f;
				AnimationData.AnimationStateIndex = StateIndex;
			}
		}
	});
}


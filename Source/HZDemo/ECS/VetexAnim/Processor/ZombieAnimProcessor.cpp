#include "ZombieAnimProcessor.h"

#include "MassLODFragments.h"
#include "MassMovementFragments.h"
#include "MassRepresentationFragments.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"
#include "ECS/VetexAnim/Traits/ZombieAnimationFragment.h"

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
					default:
						break;
				}
				
				if (Velocity.Value.Size()<15)
				{
					StateIndex = 1;
				}
				
				AnimationData.PlayRate = 1.0f;
				AnimationData.AnimationStateIndex = StateIndex;
			}
		}
	});
}


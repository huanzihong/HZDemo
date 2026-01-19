#include "ZombieAnimProcessor.h"

#include "MassLODFragments.h"
#include "MassMovementFragments.h"
#include "MassRepresentationFragments.h"
#include "ECS/VetexAnim/Traits/ZombieAnimationFragment.h"

UZombieAnimProcessor::UZombieAnimProcessor():AnimationEntityQuery(*this)
{
	
}
void UZombieAnimProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	
	AnimationEntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
	
	AnimationEntityQuery.AddRequirement<FZombieAnimationFragment>(EMassFragmentAccess::ReadWrite);
	AnimationEntityQuery.AddRequirement<FMassRepresentationFragment>(EMassFragmentAccess::ReadOnly);
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
		for (FMassExecutionContext::FEntityIterator EntityIt = Context.CreateEntityIterator(); EntityIt; ++EntityIt)
		{
			FZombieAnimationFragment& AnimationData = AnimationDataList[EntityIt];

			const FMassRepresentationFragment& Visualization = VisualizationList[EntityIt];
			const FMassVelocityFragment& Velocity = VelocityList[EntityIt];
			
			if (Visualization.CurrentRepresentation != EMassRepresentationType::None)
			{
				int32 StateIndex = 0;
				FMassEntityHandle Entity = Context.GetEntity(EntityIt);
				// @todo: Make a better way to map desired anim states here. Currently the anim texture index to access is hard-coded.
				const float VelocitySizeSq = Velocity.Value.SizeSquared();
				const bool bIsWalking = Velocity.Value.Size() > 50;
				if(bIsWalking)
				{
					StateIndex = 0;
					/*const float AuthoredAnimSpeed = 140.0f;
					const float PrevPlayRate = AnimationData.PlayRate;
					AnimationData.PlayRate = FMath::Clamp(FMath::Sqrt(VelocitySizeSq / (AuthoredAnimSpeed * AuthoredAnimSpeed)), 0.8f, 2.0f);*/
				}
				else
				{
					
					StateIndex = 1;
				}
				AnimationData.PlayRate = 1.0f;
				AnimationData.AnimationStateIndex = StateIndex;
			}
		}
	});
}


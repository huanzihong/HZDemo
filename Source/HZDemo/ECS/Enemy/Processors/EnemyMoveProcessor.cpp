#include "EnemyMoveProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassNavigationFragments.h"
#include "ECS/Enemy/Traits/BeHitTags.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"
#include "Kismet/GameplayStatics.h"

UEnemyMoveToPlayerProcessor::UEnemyMoveToPlayerProcessor():EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Movement);
}

void UEnemyMoveToPlayerProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	//不处于受击
	EntityQuery.AddTagRequirement<FKnockTag>(EMassFragmentPresence::None);
	
	EntityQuery.AddTagRequirement<FEnemyTag>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
	
}

void UEnemyMoveToPlayerProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(Context.GetWorld(), 0)->GetActorLocation();

	EntityQuery.ForEachEntityChunk(Context, [&, PlayerLocation](FMassExecutionContext& MassExecutionContext)
	{
		const TConstArrayView<FTransformFragment> TransformList = MassExecutionContext.GetFragmentView<FTransformFragment>();
		const TArrayView<FMassMoveTargetFragment> MoveTargetList = MassExecutionContext.GetMutableFragmentView<FMassMoveTargetFragment>();

		for (int EntityIndex = 0; EntityIndex < MassExecutionContext.GetNumEntities(); ++EntityIndex)
		{
			const FVector EntityLocation = TransformList[EntityIndex].GetTransform().GetLocation();

			FMassMoveTargetFragment& MassMoveTargetFragment = MoveTargetList[EntityIndex];
			MassMoveTargetFragment.Center = PlayerLocation;
			MassMoveTargetFragment.Forward = (PlayerLocation - EntityLocation).GetSafeNormal();
			MassMoveTargetFragment.DistanceToGoal = FVector::Dist(EntityLocation, MassMoveTargetFragment.Center);

			if (MassMoveTargetFragment.GetCurrentAction() == EMassMovementAction::Stand && MassMoveTargetFragment.DistanceToGoal > 50.f)
			{
				MassMoveTargetFragment.CreateNewAction(EMassMovementAction::Move, *Context.GetWorld());
				MassMoveTargetFragment.IntentAtGoal = EMassMovementAction::Stand;
			}
			else if (MassMoveTargetFragment.GetCurrentAction() == EMassMovementAction::Move && MassMoveTargetFragment.DistanceToGoal <= 50.f)
			{
				MassMoveTargetFragment.CreateNewAction(EMassMovementAction::Stand, *Context.GetWorld());
			}
		}
	});
}
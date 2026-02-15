#include "EnemyMoveProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassLODFragments.h"
#include "MassNavigationFragments.h"
#include "MassSignalSubsystem.h"
#include "MassStateTreeTypes.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"
#include "Kismet/GameplayStatics.h"

UEnemyWanderProcessor::UEnemyWanderProcessor():EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
}

void UEnemyWanderProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddTagRequirement<FChasePlayerTag>(EMassFragmentPresence::None);
	EntityQuery.AddTagRequirement<FEnemyTag>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddTagRequirement<FMassOffLODTag>(EMassFragmentPresence::None);
	EntityQuery.AddSubsystemRequirement<UMassSignalSubsystem>(EMassFragmentAccess::ReadWrite);
	
}

void UEnemyWanderProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(Context.GetWorld(), 0)->GetActorLocation();

	EntityQuery.ForEachEntityChunk(Context, [&, PlayerLocation](FMassExecutionContext& MassExecutionContext)
	{
		const TConstArrayView<FTransformFragment> TransformList = MassExecutionContext.GetFragmentView<FTransformFragment>();
		const TArrayView<FMassMoveTargetFragment> MoveTargetList = MassExecutionContext.GetMutableFragmentView<FMassMoveTargetFragment>();
		auto MassSignalSubsystem = MassExecutionContext.GetMutableSubsystem<UMassSignalSubsystem>();
		
		for (int EntityIndex = 0; EntityIndex < MassExecutionContext.GetNumEntities(); ++EntityIndex)
		{
			auto Transform = TransformList[EntityIndex];
			auto MoveTarget = MoveTargetList[EntityIndex];
			//DrawDebugSphere(GetWorld(), MoveTarget.Center, 5.f, 12, FColor::Red, false, 0.1f);
			//DrawDebugSphere(GetWorld(), Transform.GetTransform().GetLocation(), 5.0f, 12, FColor::Yellow, false, 0.1f);
			//DrawDebugSphere(Context.GetWorld(), MoveTarget.Center, 50.f, 12, FColor::Blue, false, 5.f);
			if (MoveTarget.GetCurrentAction() == EMassMovementAction::Move)
			{
				auto Distance = FVector::Dist2D(Transform.GetTransform().GetLocation(), MoveTarget.Center);
				
				//DrawDebugPoint(GetWorld(), Transform.GetTransform().GetLocation(), 50.0f, FColor::Yellow, true, 1.0f);
				//DrawDebugPoint(GetWorld(), MoveTarget.Center, 50.0f, FColor::Blue, true, 1.0f);
				if (Distance < 100.f)
				{
					//这个激活tick ontick transitions
					MassSignalSubsystem->SignalEntityDeferred(Context, UE::Mass::Signals::StateTreeActivate, Context.GetEntity(EntityIndex));	
				}
			}
		}
	});
}


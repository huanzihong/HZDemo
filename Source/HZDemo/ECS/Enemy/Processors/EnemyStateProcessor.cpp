#include "EnemyStateProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassSignalSubsystem.h"
#include "MassStateTreeTypes.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"
#include "Kismet/GameplayStatics.h"

UEnemyStateProcessor::UEnemyStateProcessor():EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
}

void UEnemyStateProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddSubsystemRequirement<UMassSignalSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddTagRequirement<FChasePlayerTag>(EMassFragmentPresence::All);
	EntityQuery.AddTagRequirement<FEnemyTag>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FEnemyFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
}

void UEnemyStateProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(Context.GetWorld(), 0)->GetActorLocation();

	EntityQuery.ForEachEntityChunk(Context, [this,PlayerLocation](FMassExecutionContext& Context)
	{
		const TConstArrayView<FTransformFragment> TransformList = Context.GetFragmentView<FTransformFragment>();
		const TArrayView<FEnemyFragment> EnemyList = Context.GetMutableFragmentView<FEnemyFragment>();
		auto SignalSubsystem = Context.GetMutableSubsystem<UMassSignalSubsystem>();
		for (int EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
		{
			FEnemyFragment& EnemyFragment = EnemyList[EntityIndex];
			const FTransformFragment TransformFragment = TransformList[EntityIndex];

			if((PlayerLocation-TransformFragment.GetTransform().GetLocation()).Length()>5000)
			{
				EnemyFragment.EnemyState = EEnemyState::Other;
				Context.Defer().RemoveTag<FChasePlayerTag>(Context.GetEntity(EntityIndex));
				SignalSubsystem->SignalEntity(UE::Mass::Signals::StateTreeActivate,Context.GetEntity(EntityIndex));
			}else
			{
				//statetree需要收到信号后才会去tick
				SignalSubsystem->SignalEntity(UE::Mass::Signals::StateTreeActivate,Context.GetEntity(EntityIndex));
			}
		}
	});
}


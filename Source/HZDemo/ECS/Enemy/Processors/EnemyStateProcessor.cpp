#include "EnemyStateProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassNavMeshNavigationFragments.h"
#include "MassSignalSubsystem.h"
#include "MassStateTreeTypes.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"
#include "Kismet/GameplayStatics.h"
#include "MassRepresentationFragments.h"

UEnemyStateInitializer::UEnemyStateInitializer():EntityQuery(*this)
{
	ObservedType = FEnemyTag::StaticStruct();
	Operation = EMassObservedOperation::Add;
}

void UEnemyStateInitializer::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FEnemyFragment>(EMassFragmentAccess::ReadWrite);
}

void UEnemyStateInitializer::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(Context.GetWorld(), 0)->GetActorLocation();
	EntityQuery.ForEachEntityChunk(Context, [this, PlayerLocation](FMassExecutionContext& Context)
	{
		const TConstArrayView<FTransformFragment> TransformFragments = Context.GetFragmentView<FTransformFragment>();
		const TArrayView<FEnemyFragment> EnemyFragments = Context.GetMutableFragmentView<FEnemyFragment>();
		
		const int32 NumEntities = Context.GetNumEntities();
		
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& EnemyFragment = EnemyFragments[EntityIdx];
			const FTransformFragment TransformFragment = TransformFragments[EntityIdx];
			const FVector Location = TransformFragment.GetTransform().GetLocation();
			auto Distance = (PlayerLocation-Location).Length();	
			if(Distance>5000)
			{
				EnemyFragment.EnemyState = EEnemyState::Wander;
			}else
			{
				EnemyFragment.EnemyState = EEnemyState::ChasePlayer;
				Context.Defer().AddTag<FChasePlayerTag>(Context.GetEntity(EntityIdx));
			}
		}
		
	});
}

UEnemyStateProcessor::UEnemyStateProcessor():EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Movement);
}

void UEnemyStateProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddSubsystemRequirement<UMassSignalSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<UHashGridSubsystem>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddTagRequirement<FEnemyTag>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FEnemyFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassNavMeshShortPathFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassRepresentationFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddTagRequirement<FMassLowLODTag>(EMassFragmentPresence::None);
	EntityQuery.AddTagRequirement<FMassOffLODTag>(EMassFragmentPresence::None);
	EntityQuery.AddChunkRequirement<FMassVisualizationChunkFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.SetChunkFilter(&FMassVisualizationChunkFragment::AreAnyEntitiesVisibleInChunk);
	
}

void UEnemyStateProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(Context.GetWorld(), 0)->GetActorLocation();

	
	EntityQuery.ForEachEntityChunk(Context, [this,PlayerLocation](FMassExecutionContext& Context)
	{
		const TConstArrayView<FTransformFragment> TransformList = Context.GetFragmentView<FTransformFragment>();
		const TConstArrayView<FMassVelocityFragment> VelocityList = Context.GetFragmentView<FMassVelocityFragment>();
		const TArrayView<FEnemyFragment> EnemyList = Context.GetMutableFragmentView<FEnemyFragment>();
		const TArrayView<FMassNavMeshShortPathFragment> NavMeshList = Context.GetMutableFragmentView<FMassNavMeshShortPathFragment>();
		//const TConstArrayView<FMassRepresentationFragment> RepresentationList = Context.GetFragmentView<FMassRepresentationFragment>();
		auto SignalSubsystem = Context.GetMutableSubsystem<UMassSignalSubsystem>();
		for (int EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
		{
			/*const FMassRepresentationFragment RepresentationFragment = RepresentationList[EntityIndex];
			if (RepresentationFragment.CurrentRepresentation != EMassRepresentationType::HighResSpawnedActor)
			{
				return;
			}*/
			
			
			
			FMassNavMeshShortPathFragment& NavMeshFragment = NavMeshList[EntityIndex];
			FEnemyFragment& EnemyFragment = EnemyList[EntityIndex];
			const FTransformFragment TransformFragment = TransformList[EntityIndex];
			const FMassVelocityFragment VelocityFragment = VelocityList[EntityIndex];
			
			
			if (EnemyFragment.EnemyState == EEnemyState::Knock)
			{
				return;
			}
			
			auto Distance = (PlayerLocation-TransformFragment.GetTransform().GetLocation()).Length();
			if(Distance>5000)
			{
				NavMeshFragment.Reset();
				if (EnemyFragment.EnemyState == EEnemyState::ChasePlayer)
				{
					Context.Defer().RemoveTag<FChasePlayerTag>(Context.GetEntity(EntityIndex));
				}
				EnemyFragment.EnemyState = EEnemyState::Wander;
				
			}else
			{
				if (EnemyFragment.EnemyState != EEnemyState::ChasePlayer)
				{
					Context.Defer().AddTag<FChasePlayerTag>(Context.GetEntity(EntityIndex));
				}
				EnemyFragment.EnemyState = EEnemyState::ChasePlayer;
				
				//statetree需要收到信号后才会去tick
				//SignalSubsystem->SignalEntity(UE::Mass::Signals::StateTreeActivate,Context.GetEntity(EntityIndex));
			}
			
			SignalSubsystem->SignalEntity(UE::Mass::Signals::StateTreeActivate,Context.GetEntity(EntityIndex));
		}
		
	});
}


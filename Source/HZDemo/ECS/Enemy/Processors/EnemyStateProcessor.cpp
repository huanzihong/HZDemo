#include "EnemyStateProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassNavMeshNavigationFragments.h"
#include "MassSignalSubsystem.h"
#include "MassStateTreeTypes.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"
#include "MassRepresentationFragments.h"
#include "Engine/World.h"

namespace
{
	bool FindNearestPlayerPawn(UWorld* World, const FVector& FromLocation, FVector& OutLocation)
	{
		if (!World)
		{
			return false;
		}

		float BestDistSq = TNumericLimits<float>::Max();
		bool bFound = false;

		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			const APlayerController* PC = It->Get();
			const APawn* Pawn = PC ? PC->GetPawn() : nullptr;
			if (!IsValid(Pawn))
			{
				continue;
			}

			const FVector PawnLocation = Pawn->GetActorLocation();
			const float DistSq = FVector::DistSquared2D(FromLocation, PawnLocation);
			if (DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				OutLocation = PawnLocation;
				bFound = true;
			}
		}

		return bFound;
	}
}

UEnemyStateInitializer::UEnemyStateInitializer():EntityQuery(*this)
{
	ObservedType = FEnemyTag::StaticStruct();
	ObservedOperations = EMassObservedOperationFlags::Add;
}

void UEnemyStateInitializer::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FEnemyFragment>(EMassFragmentAccess::ReadWrite);
}

void UEnemyStateInitializer::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	UWorld* World = Context.GetWorld();
	EntityQuery.ForEachEntityChunk(Context, [this, World](FMassExecutionContext& Context)
	{
		const TConstArrayView<FTransformFragment> TransformFragments = Context.GetFragmentView<FTransformFragment>();
		const TArrayView<FEnemyFragment> EnemyFragments = Context.GetMutableFragmentView<FEnemyFragment>();
		
		const int32 NumEntities = Context.GetNumEntities();
		
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& EnemyFragment = EnemyFragments[EntityIdx];
			const FTransformFragment TransformFragment = TransformFragments[EntityIdx];
			const FVector Location = TransformFragment.GetTransform().GetLocation();
			FVector PlayerLocation = FVector::ZeroVector;
			const bool bHasPlayer = FindNearestPlayerPawn(World, Location, PlayerLocation);
			if (!bHasPlayer)
			{
				EnemyFragment.EnemyState = EEnemyState::Wander;
				continue;
			}
			auto Distance = (PlayerLocation-Location).Length();
			if(Distance>5000)
			{
				EnemyFragment.EnemyState = EEnemyState::Wander;
			}else
			{
				EnemyFragment.EnemyState = EEnemyState::ChasePlayer;
				//Context.Defer().AddTag<FChasePlayerTag>(Context.GetEntity(EntityIdx));
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
	UWorld* World = Context.GetWorld();
	EntityQuery.ForEachEntityChunk(Context, [this, World](FMassExecutionContext& Context)
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

			FVector PlayerLocation = FVector::ZeroVector;
			const bool bHasPlayer = FindNearestPlayerPawn(World, TransformFragment.GetTransform().GetLocation(), PlayerLocation);
			if (!bHasPlayer)
			{
				NavMeshFragment.Reset();
				EnemyFragment.EnemyState = EEnemyState::Wander;
				SignalSubsystem->SignalEntity(UE::Mass::Signals::StateTreeActivate,Context.GetEntity(EntityIndex));
				continue;
			}
			
			auto Distance = (PlayerLocation-TransformFragment.GetTransform().GetLocation()).Length();
			if(Distance>5000)
			{
				NavMeshFragment.Reset();
				/*if (EnemyFragment.EnemyState == EEnemyState::ChasePlayer)
				{
					Context.Defer().RemoveTag<FChasePlayerTag>(Context.GetEntity(EntityIndex));
				}*/
				EnemyFragment.EnemyState = EEnemyState::Wander;
				
			}else
			{
				/*if (EnemyFragment.EnemyState != EEnemyState::ChasePlayer)
				{
					Context.Defer().AddTag<FChasePlayerTag>(Context.GetEntity(EntityIndex));
				}*/
				EnemyFragment.EnemyState = EEnemyState::ChasePlayer;
				
				//statetree需要收到信号后才会去tick
				//SignalSubsystem->SignalEntity(UE::Mass::Signals::StateTreeActivate,Context.GetEntity(EntityIndex));
			}
			
			SignalSubsystem->SignalEntity(UE::Mass::Signals::StateTreeActivate,Context.GetEntity(EntityIndex));
		}
		
	});
}


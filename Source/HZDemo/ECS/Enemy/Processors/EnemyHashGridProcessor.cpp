#include "EnemyHashGridProcessor.h"

#include "MassActorSubsystem.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassRepresentationFragments.h"
#include "MassSignalSubsystem.h"
#include "MassStateTreeTypes.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"
#include "Kismet/GameplayStatics.h"

UEnemyInitializer::UEnemyInitializer(): EntityQuery(*this)
{
	ObservedType = FEnemyTag::StaticStruct();
	Operation = EMassObservedOperation::Add;
}

void UEnemyInitializer::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FEnemyFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<UHashGridSubsystem>(EMassFragmentAccess::ReadWrite);
}

void UEnemyInitializer::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
	{
		auto TransformFragments = Context.GetFragmentView<FTransformFragment>();
		auto EnemyFragments = Context.GetMutableFragmentView<FEnemyFragment>();
		auto BulletHellSubsystem = Context.GetMutableSubsystem<UHashGridSubsystem>();
		auto& HashGrid = BulletHellSubsystem->GetHashGrid_Mutable();
		
		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& EnemyFragment = EnemyFragments[EntityIdx];
			auto TransformFragment = TransformFragments[EntityIdx];
			auto Location = TransformFragment.GetTransform().GetLocation();
			
			// 使用胶囊体范围构建AABB用于HashGrid
			FVector Extent(EnemyFragment.CapsuleRadius, EnemyFragment.CapsuleRadius, EnemyFragment.CapsuleHalfHeight);
			EnemyFragment.CellLocation = HashGrid.Add(Context.GetEntity(EntityIdx), FBox::BuildAABB(Location+FVector{0,0,90},Extent));
		}
	});
}

UEnemyDestructor::UEnemyDestructor(): EntityQuery(*this)
{
	ObservedType = FEnemyFragment::StaticStruct();
	Operation = EMassObservedOperation::Remove;
}

void UEnemyDestructor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FEnemyFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<UHashGridSubsystem>(EMassFragmentAccess::ReadWrite);
}

void UEnemyDestructor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
	{
		auto EnemyFragments = Context.GetFragmentView<FEnemyFragment>();
		auto BulletHellSubsystem = Context.GetMutableSubsystem<UHashGridSubsystem>();
		auto& HashGrid = BulletHellSubsystem->GetHashGrid_Mutable();
		
		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& EnemyFragment = EnemyFragments[EntityIdx];
			
			HashGrid.Remove(Context.GetEntity(EntityIdx), EnemyFragment.CellLocation);
		}
	});
}

UpdateEnemyHashGridProcessor::UpdateEnemyHashGridProcessor():UpdateHashGridQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
}

void UpdateEnemyHashGridProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	UpdateHashGridQuery.AddRequirement<FEnemyFragment>(EMassFragmentAccess::ReadWrite);
	UpdateHashGridQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	UpdateHashGridQuery.AddSubsystemRequirement<UHashGridSubsystem>(EMassFragmentAccess::ReadWrite);
	UpdateHashGridQuery.AddSubsystemRequirement<UMassSignalSubsystem>(EMassFragmentAccess::ReadWrite);
	UpdateHashGridQuery.AddChunkRequirement<FMassVisualizationChunkFragment>(EMassFragmentAccess::ReadOnly);
	UpdateHashGridQuery.SetChunkFilter(&FMassVisualizationChunkFragment::AreAnyEntitiesVisibleInChunk);
}

void UpdateEnemyHashGridProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(Context.GetWorld(), 0)->GetActorLocation();
	UpdateHashGridQuery.ForEachEntityChunk(Context, [this,PlayerLocation](FMassExecutionContext& Context)
	{
		SCOPED_NAMED_EVENT(UpdateHashGrid, FColor::Green);
		auto HashGridSubsystem = Context.GetMutableSubsystem<UHashGridSubsystem>();
		//auto MassSignalSubsystem= Context.GetMutableSubsystem<UMassSignalSubsystem>();
		auto BHEnemyFragments = Context.GetMutableFragmentView<FEnemyFragment>();
		auto TransformFragments = Context.GetFragmentView<FTransformFragment>();
		
		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto TransformFragment = TransformFragments[EntityIdx];
			auto& BHEnemyFragment = BHEnemyFragments[EntityIdx];

			auto Location = TransformFragment.GetTransform().GetLocation();

			// 使用胶囊体范围构建AABB用于HashGrid
			FVector Extent(BHEnemyFragment.CapsuleRadius, BHEnemyFragment.CapsuleRadius, BHEnemyFragment.CapsuleHalfHeight);
			BHEnemyFragment.CellLocation = HashGridSubsystem->GetHashGrid_Mutable().Move(Context.GetEntity(EntityIdx), BHEnemyFragment.CellLocation, FBox::BuildAABB(Location+FVector{0,0,90},Extent));

			// Debug绘制胶囊体
			//DrawDebugCapsule(Context.GetWorld(), Location+FVector{0,0,90}, BHEnemyFragment.CapsuleHalfHeight, BHEnemyFragment.CapsuleRadius, FQuat::Identity, FColor::Green, false, -1.f, 0, 1.f);
		}

		/*TArray<FMassEntityHandle> Entities;
			HashGridSubsystem->GetHashGrid().Query(FBox::BuildAABB(PlayerLocation, FVector(2500.f)), Entities);

		for(auto Entity: Entities)
		{
			auto EnemyFragment = Context.GetEntityManagerChecked().GetFragmentDataPtr<FEnemyFragment>(Entity);
			if(EnemyFragment->EnemyState != EEnemyState::ChasePlayer)
			{
				MassSignalSubsystem->SignalEntity(UE::Mass::Signals::StateTreeActivate,Entity);
				EnemyFragment->EnemyState = EEnemyState::ChasePlayer;
				Context.Defer().AddTag<FChasePlayerTag>(Entity);
			}
		}*/
	});
}
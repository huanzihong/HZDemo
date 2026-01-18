#include "EnemyHashGridProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"

UEnemyInitializer::UEnemyInitializer(): EntityQuery(*this)
{
	ObservedType = FEnemyTag::StaticStruct();
	Operation = EMassObservedOperation::Add;
}

void UEnemyInitializer::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FEnemyFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<UEnemyHashGridSubsystem>(EMassFragmentAccess::ReadWrite);
}

void UEnemyInitializer::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this](FMassExecutionContext& Context)
	{
		auto TransformFragments = Context.GetFragmentView<FTransformFragment>();
		auto EnemyFragments = Context.GetMutableFragmentView<FEnemyFragment>();
		auto BulletHellSubsystem = Context.GetMutableSubsystem<UEnemyHashGridSubsystem>();
		auto& HashGrid = BulletHellSubsystem->GetHashGrid_Mutable();
		
		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& EnemyFragment = EnemyFragments[EntityIdx];
			auto TransformFragment = TransformFragments[EntityIdx];
			auto Location = TransformFragment.GetTransform().GetLocation();
			
			EnemyFragment.CellLocation = HashGrid.Add(Context.GetEntity(EntityIdx), FBox::BuildAABB(Location, EnemyFragment.CollisionExtent));
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
	EntityQuery.AddSubsystemRequirement<UEnemyHashGridSubsystem>(EMassFragmentAccess::ReadWrite);
}

void UEnemyDestructor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
	{
		auto EnemyFragments = Context.GetFragmentView<FEnemyFragment>();
		auto BulletHellSubsystem = Context.GetMutableSubsystem<UEnemyHashGridSubsystem>();
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
	UpdateHashGridQuery.AddSubsystemRequirement<UEnemyHashGridSubsystem>(EMassFragmentAccess::ReadWrite);
}

void UpdateEnemyHashGridProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	UpdateHashGridQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
	{
		SCOPED_NAMED_EVENT(STAT_UpdateHashGrid, FColor::Red);
		auto BulletHellSubsystem = Context.GetMutableSubsystem<UEnemyHashGridSubsystem>();
		auto BHEnemyFragments = Context.GetMutableFragmentView<FEnemyFragment>();
		auto TransformFragments = Context.GetFragmentView<FTransformFragment>();
		
		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto TransformFragment = TransformFragments[EntityIdx];
			auto& BHEnemyFragment = BHEnemyFragments[EntityIdx];

			auto Location = TransformFragment.GetTransform().GetLocation();

			BHEnemyFragment.CellLocation = BulletHellSubsystem->GetHashGrid_Mutable().Move(Context.GetEntity(EntityIdx), BHEnemyFragment.CellLocation, FBox::BuildAABB(Location, BHEnemyFragment.CollisionExtent));
		}
	});
}
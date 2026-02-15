#include "WeaponHashGridProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "ECS/Weapon/Traits/WeaponFragment.h"
#include "ECS/Enemy/Subsystem/HashGridSubsystem.h"

UWeaponInitializer::UWeaponInitializer(): EntityQuery(*this)
{
	ObservedType = FWeaponTag::StaticStruct();
	Operation = EMassObservedOperation::Add;
}

void UWeaponInitializer::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FWeaponFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<UHashGridSubsystem>(EMassFragmentAccess::ReadWrite);
}

void UWeaponInitializer::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
	{
		auto TransformFragments = Context.GetFragmentView<FTransformFragment>();
		auto WeaponFragments = Context.GetMutableFragmentView<FWeaponFragment>();
		auto HashGridSubsystem = Context.GetMutableSubsystem<UHashGridSubsystem>();
		auto& WeaponHashGrid = HashGridSubsystem->GetWeaponHashGrid_Mutable();

		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& WeaponFragment = WeaponFragments[EntityIdx];
			auto TransformFragment = TransformFragments[EntityIdx];
			auto Location = TransformFragment.GetTransform().GetLocation();

			// 使用小范围构建AABB用于HashGrid
			FVector Extent(50.f, 50.f, 50.f);
			WeaponFragment.CellLocation = WeaponHashGrid.Add(Context.GetEntity(EntityIdx), FBox::BuildAABB(Location, Extent));
		}
	});
}

UWeaponDestructor::UWeaponDestructor(): EntityQuery(*this)
{
	ObservedType = FWeaponFragment::StaticStruct();
	Operation = EMassObservedOperation::Remove;
}

void UWeaponDestructor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FWeaponFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<UHashGridSubsystem>(EMassFragmentAccess::ReadWrite);
}

void UWeaponDestructor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
	{
		auto WeaponFragments = Context.GetFragmentView<FWeaponFragment>();
		auto HashGridSubsystem = Context.GetMutableSubsystem<UHashGridSubsystem>();
		auto& WeaponHashGrid = HashGridSubsystem->GetWeaponHashGrid_Mutable();

		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& WeaponFragment = WeaponFragments[EntityIdx];

			WeaponHashGrid.Remove(Context.GetEntity(EntityIdx), WeaponFragment.CellLocation);
		}
	});
}



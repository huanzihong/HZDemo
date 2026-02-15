#include "AIFindWeaponTask.h"

#include "MassActorSubsystem.h"
#include "MassCommonFragments.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "ECS/Weapon/Traits/WeaponFragment.h"

EStateTreeRunStatus FAIFindWeaponTask::EnterState(FStateTreeExecutionContext& Context,
                                                  const FStateTreeTransitionResult& Transition) const
{
	const AActor& AIActor = Context.GetExternalData(ActorHandle);
	const UHashGridSubsystem& HashGridSubsystem = Context.GetExternalData(HashGridSubsystemHandle);
	const FWeaponHashGrid& WeaponHashGrid = HashGridSubsystem.GetWeaponHashGrid();
	const UMassEntitySubsystem& EntitySubsystem = Context.GetExternalData(EntitySubsystemHandle);
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const FVector CurrentLocation = AIActor.GetActorLocation();

	// 构建搜索范围
	FBox SearchBox = FBox::BuildAABB(CurrentLocation, FVector(InstanceData.SerchRadius));

	// 查询范围内的所有武器
	TArray<FMassEntityHandle> NearbyWeapons;
	WeaponHashGrid.Query(SearchBox, NearbyWeapons);

	if (NearbyWeapons.Num() == 0)
	{
		InstanceData.bFindWeapon = false;
		return EStateTreeRunStatus::Failed;
	}

	// 找到最近的武器
	float ClosestDistanceSq = FLT_MAX;
	FMassEntityHandle ClosestWeapon;
	FVector ClosestWeaponLocation{};
	for (const FMassEntityHandle& WeaponEntity : NearbyWeapons)
	{
		if (!EntitySubsystem.GetEntityManager().IsEntityValid(WeaponEntity))
			continue;

		// 检查武器是否被拾取
		const FWeaponFragment* WeaponFragment = EntitySubsystem.GetEntityManager().GetFragmentDataPtr<FWeaponFragment>(WeaponEntity);
		if (!WeaponFragment || WeaponFragment->bIsPickedUp)
			continue;

		const FTransformFragment* WeaponTransform = EntitySubsystem.GetEntityManager().GetFragmentDataPtr<FTransformFragment>(WeaponEntity);
		if (!WeaponTransform)
			continue;

		const FVector WeaponLocation = WeaponTransform->GetTransform().GetLocation();
		const float DistanceSq = FVector::DistSquared(CurrentLocation, WeaponLocation);

		if (DistanceSq < ClosestDistanceSq)
		{
			ClosestWeaponLocation = WeaponLocation;
			ClosestDistanceSq = DistanceSq;
			ClosestWeapon = WeaponEntity;
		}
	}

	if (ClosestWeapon.IsValid())
	{
		InstanceData.bFindWeapon = true;
		InstanceData.WeaponLocation = ClosestWeaponLocation;
		FMassActorFragment* ActorFragment = EntitySubsystem.GetEntityManager().GetFragmentDataPtr<FMassActorFragment>(ClosestWeapon);
		InstanceData.WeaponActor = Cast<AWeapon>(ActorFragment->GetMutable());
		return EStateTreeRunStatus::Running;
	}

	InstanceData.bFindWeapon = false;
	return EStateTreeRunStatus::Failed;
}

bool FAIFindWeaponTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(EntitySubsystemHandle);
	Linker.LinkExternalData(HashGridSubsystemHandle);
	Linker.LinkExternalData(ActorHandle);
	return FStateTreeAITaskBase::Link(Linker);
}

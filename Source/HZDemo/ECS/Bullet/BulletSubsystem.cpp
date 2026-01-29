#include "BulletSubsystem.h"

#include "BulletFragment.h"
#include "MassEntitySubsystem.h"
#include "MassSignalSubsystem.h"
#include "MassSpawnerSubsystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


void UBulletSubsystem::SpawnBullet(UMassEntityConfigAsset* BulletConfig, const FVector& Location,
                                     const FVector& Direction)
{
	check(BulletConfig);
	auto SignalSubsystem = GetWorld()->GetSubsystem<UMassSignalSubsystem>();
	auto SpawnerSystem = GetWorld()->GetSubsystem<UMassSpawnerSubsystem>();
	auto& EntityManager = GetWorld()->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();

	TArray<FMassEntityHandle> EntitiesSpawned;
	SpawnerSystem->SpawnEntities(BulletConfig->GetOrCreateEntityTemplate(*GetWorld()), 1, EntitiesSpawned);

	auto& BulletFragment = EntityManager.GetFragmentDataChecked<FBulletFragment>(EntitiesSpawned[0]);
	BulletFragment.Direction = Direction;
	BulletFragment.SpawnLocation = Location;

	// Get trail effect from config
	UNiagaraSystem* TrailSystem = nullptr;
	
	
	if (BulletFragment.TrailEffectAsset.IsPending())
	{
		TrailSystem = BulletFragment.TrailEffectAsset.LoadSynchronous();
	}else
	{
		TrailSystem = BulletFragment.TrailEffectAsset.Get();
	}
	
	// Spawn trail effect if configured
	if (TrailSystem)
	{
		UNiagaraComponent* TrailComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			TrailSystem,
			Location,
			Direction.Rotation(),
			FVector(1.0f),
			true,
			true,
			ENCPoolMethod::None,
			true
		);

		if (TrailComponent)
		{
			BulletFragment.TrailEffect = TrailComponent;
		}
	}

	SignalSubsystem->SignalEntity(BulletHell::Signals::BulletSpawned, EntitiesSpawned[0]);
}

void UBulletSubsystem::SpawnBulletWithTrail(UMassEntityConfigAsset* BulletConfig, UNiagaraSystem* TrailSystem,
	const FVector& Location, const FVector& Direction)
{
	check(BulletConfig);
	auto SignalSubsystem = GetWorld()->GetSubsystem<UMassSignalSubsystem>();
	auto SpawnerSystem = GetWorld()->GetSubsystem<UMassSpawnerSubsystem>();
	auto& EntityManager = GetWorld()->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();

	TArray<FMassEntityHandle> EntitiesSpawned;
	SpawnerSystem->SpawnEntities(BulletConfig->GetOrCreateEntityTemplate(*GetWorld()), 1, EntitiesSpawned);

	auto& BulletFragment = EntityManager.GetFragmentDataChecked<FBulletFragment>(EntitiesSpawned[0]);
	BulletFragment.Direction = Direction;
	BulletFragment.SpawnLocation = Location;

	// Spawn trail effect if provided
	if (TrailSystem)
	{
		UNiagaraComponent* TrailComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			TrailSystem,
			Location,
			Direction.Rotation(),
			FVector(1.0f),
			true,
			true,
			ENCPoolMethod::None,
			true
		);

		if (TrailComponent)
		{
			BulletFragment.TrailEffect = TrailComponent;
		}
	}

	SignalSubsystem->SignalEntity(BulletHell::Signals::BulletSpawned, EntitiesSpawned[0]);
}

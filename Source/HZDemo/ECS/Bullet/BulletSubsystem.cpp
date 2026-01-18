#include "BulletSubsystem.h"

#include "BulletFragment.h"
#include "MassEntitySubsystem.h"
#include "MassSignalSubsystem.h"
#include "MassSpawnerSubsystem.h"


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

	SignalSubsystem->SignalEntity(BulletHell::Signals::BulletSpawned, EntitiesSpawned[0]);
}

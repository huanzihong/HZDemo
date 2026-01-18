#pragma once
#include "MassEntityConfigAsset.h"

#include "BulletSubsystem.generated.h"
namespace BulletHell::Signals
{
	const FName BulletSpawned = FName(TEXT("BulletSpawned"));
	const FName BulletDestroy = FName(TEXT("BulletDestroy"));
}

UCLASS()
class HZDEMO_API UBulletSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable)
	void SpawnBullet(UMassEntityConfigAsset* BulletConfig, const FVector& Location, const FVector& Direction);
	
};

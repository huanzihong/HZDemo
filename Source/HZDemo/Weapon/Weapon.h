#pragma once
#include "Weapon.generated.h"

class UMassEntityConfigAsset;

UCLASS()
class AWeapon : public AActor
{
	GENERATED_BODY()
public:
	AWeapon();
	UMassEntityConfigAsset* GetBulletConfig();

	void SetWeaponState(bool bEquipped);
	
	USkeletalMeshComponent* GetWeaponMesh();
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	

	
private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, Category="Bullet")
	UMassEntityConfigAsset* BulletConfig;
};

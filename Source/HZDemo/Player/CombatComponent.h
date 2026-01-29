#pragma once
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	AWeapon* GetEquippedWeapon();

	void EquipWeapon(AWeapon* InWeapon);
	
	void SetAimming(bool bAim);

	bool GetAimming()
	{
		return bAimming;
	};

	FORCEINLINE void SetOverlappingWeapon(AWeapon* Weapon) { OverlappingWeapon = Weapon; }
	FORCEINLINE AWeapon* GetOverlappingWeapon() const { return OverlappingWeapon; }

private:
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	TObjectPtr<AWeapon> OverlappingWeapon;

	bool bAimming;
	
};

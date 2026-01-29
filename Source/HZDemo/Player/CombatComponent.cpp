#include "CombatComponent.h"
#include "Weapon/Weapon.h"
#include "GameFramework/Character.h"

AWeapon* UCombatComponent::GetEquippedWeapon()
{
	return EquippedWeapon;
}

void UCombatComponent::EquipWeapon(AWeapon* InWeapon)
{
	if (InWeapon == nullptr) return;

	EquippedWeapon = InWeapon;
	EquippedWeapon->SetOwner(GetOwner());
	EquippedWeapon->SetWeaponState(true);

	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		EquippedWeapon->AttachToComponent(Character->GetMesh(), AttachmentRules, FName("Weapon"));
	}
}

void UCombatComponent::SetAimming(bool bAim)
{
	bAimming = bAim;
}

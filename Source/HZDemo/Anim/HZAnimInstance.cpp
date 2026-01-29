#include "HZAnimInstance.h"

#include "HZDemoCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/CombatComponent.h"

void UHZAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	HZCharacter = Cast<AHZDemoCharacter>(TryGetPawnOwner());
}

void UHZAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (HZCharacter == nullptr)
	{
		HZCharacter = Cast<AHZDemoCharacter>(TryGetPawnOwner());
	}
	if (HZCharacter == nullptr) return;

	FVector Velocity = HZCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bInAir = HZCharacter->GetCharacterMovement()->IsFalling();
	bAccelerating = HZCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

	if(auto CombatComponent = HZCharacter->GetCombatComponent())
	{
		bAiming = CombatComponent->GetAimming();
		bEquip = CombatComponent->GetEquippedWeapon() != nullptr;
	}

	
}

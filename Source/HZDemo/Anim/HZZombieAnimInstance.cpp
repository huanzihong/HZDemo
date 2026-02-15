#include "HZZombieAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/HZZombie.h"

void UHZZombieAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	ZombieCharacter = Cast<AHZZombie>(TryGetPawnOwner());
}

void UHZZombieAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (ZombieCharacter == nullptr)
	{
		ZombieCharacter = Cast<AHZZombie>(TryGetPawnOwner());
	}
	if (ZombieCharacter == nullptr) return;

	FVector Velocity = ZombieCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();
	bMoving = Speed > 0.f;

	UCharacterMovementComponent* Movement = ZombieCharacter->GetCharacterMovement();
	if (Movement)
	{
		bInAir = Movement->IsFalling();
		bAccelerating = Movement->GetCurrentAcceleration().Size() > 0.f;
	}
	else
	{
		bInAir = false;
		bAccelerating = false;
	}
}

void UHZZombieAnimInstance::SetKnockedBack(bool KnockedBack)
{
	bKnockedBack = KnockedBack;
}

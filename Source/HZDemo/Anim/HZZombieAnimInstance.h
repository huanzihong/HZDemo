#pragma once
#include "HZZombieAnimInstance.generated.h"

class AHZZombie;
class ACharacter;

UCLASS()
class UHZZombieAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	void SetKnockedBack(bool KnockedBack);

private:
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AHZZombie> ZombieCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bInAir = false;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bAccelerating = false;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bMoving = false;

	UPROPERTY(BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bKnockedBack = false;
};

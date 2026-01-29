#pragma once
#include "HZAnimInstance.generated.h"
class AHZDemoCharacter;

UCLASS()
class UHZAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AHZDemoCharacter>  HZCharacter;
	
	UPROPERTY(BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	bool bAiming = false;

	UPROPERTY(BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	bool bEquip = false;

	UPROPERTY(BlueprintReadOnly, Category = Movement,meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bInAir;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bAccelerating;
	
};

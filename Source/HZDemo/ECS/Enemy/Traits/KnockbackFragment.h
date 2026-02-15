#pragma once
#include "MassEntityElementTypes.h"
#include "KnockbackFragment.generated.h"

USTRUCT()
struct HZDEMO_API FKnockbackFragment : public FMassFragment
{
	GENERATED_BODY()

	// 击退方向
	FVector Direction = FVector::ZeroVector;
	UPROPERTY(EditAnywhere)
	// 击退力量
	float Force = 0.0f;
	
	// 击退开始时间
	float StartTime = 0.0f;
    
	// 击退持续时间
	UPROPERTY(EditAnywhere)
	float Duration = 3.0f;
    
	// 原始速度（用于击退后恢复）
	FVector OriginalVelocity = FVector::ZeroVector;
};

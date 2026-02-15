#pragma once
#include "MassEntityElementTypes.h"
#include "MassEntityTraitBase.h"
#include "ECS/Enemy/Subsystem/HashGridSubsystem.h"
#include "EnemyFragment.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Wander,
	ChasePlayer,
	Knock,
	None,
};

USTRUCT()
struct HZDEMO_API FEnemyFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Health=100;

	// 胶囊体碰撞参数
	UPROPERTY(EditAnywhere)
	float CapsuleRadius = 50.f;

	UPROPERTY(EditAnywhere)
	float CapsuleHalfHeight = 90.f;
	
	UPROPERTY(EditAnywhere)
	EEnemyState EnemyState = EEnemyState::None;

	FEnemyHashGrid::FCellLocation CellLocation;
};

USTRUCT()
struct HZDEMO_API FEnemyTag : public FMassTag
{
	GENERATED_BODY()
};

USTRUCT()
struct HZDEMO_API FChasePlayerTag : public FMassTag
{
	GENERATED_BODY()
};

UCLASS()
class HZDEMO_API UEnemyTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:

	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	UPROPERTY(Category="Enemy", EditAnywhere)
	FEnemyFragment EnemyFragment;
};

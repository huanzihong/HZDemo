#pragma once
#include "MassEntityElementTypes.h"
#include "MassEntityTraitBase.h"
#include "ECS/Enemy/Subsystem/EnemyHashGridSubsystem.h"
#include "EnemyFragment.generated.h"

USTRUCT()
struct HZDEMO_API FEnemyFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Health;

	UPROPERTY(EditAnywhere)
	FVector CollisionExtent = FVector(100.f);

	FEnemyHashGrid::FCellLocation CellLocation;
};

USTRUCT()
struct HZDEMO_API FEnemyTag : public FMassTag
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

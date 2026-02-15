#pragma once
#include "MassEntityElementTypes.h"
#include "MassEntityTraitBase.h"
#include "ECS/Enemy/Subsystem/HashGridSubsystem.h"
#include "WeaponFragment.generated.h"

USTRUCT()
struct HZDEMO_API FWeaponFragment : public FMassFragment
{
	GENERATED_BODY()

	FWeaponHashGrid::FCellLocation CellLocation;

	// 武器是否被拾取
	bool bIsPickedUp = false;
	
};

USTRUCT()
struct HZDEMO_API FWeaponTag : public FMassTag
{
	GENERATED_BODY()
};

UCLASS()
class HZDEMO_API UWeaponTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	UPROPERTY(Category="Weapon", EditAnywhere)
	FWeaponFragment WeaponFragment;
};

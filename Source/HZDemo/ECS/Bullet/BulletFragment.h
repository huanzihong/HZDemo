#pragma once
#include "MassEntityElementTypes.h"
#include "MassEntityTraitBase.h"
#include "BulletFragment.generated.h"

UENUM(BlueprintType)
enum class EBulletHitEffect : uint8
{
	KnockUp,
};

USTRUCT()
struct FBulletFragment : public FMassFragment
{
	GENERATED_BODY()

	FVector SpawnLocation;
	
	FVector Direction;

	UPROPERTY(EditAnywhere)
	float Speed = 500.f;

	UPROPERTY(EditAnywhere)
	float Lifetime = 5.f;

	UPROPERTY(EditAnywhere)
	float DestructForce = 1000.f;

	UPROPERTY(EditAnywhere)
	EBulletHitEffect BulletHitEffect = EBulletHitEffect::KnockUp;

	
};

USTRUCT()
struct FBulletTag : public FMassTag
{
	GENERATED_BODY()
	
};

UCLASS()
class UBulletTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	UPROPERTY(EditAnywhere)
	FBulletFragment BulletFragment;
};
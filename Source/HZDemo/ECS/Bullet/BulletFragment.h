#pragma once
#include "MassEntityElementTypes.h"
#include "MassEntityTraitBase.h"
#include "NiagaraComponent.h"
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
	float Lifetime = 15.f;

	UPROPERTY(EditAnywhere)
	float DestructForce = 1000.f;

	UPROPERTY(EditAnywhere)
	EBulletHitEffect BulletHitEffect = EBulletHitEffect::KnockUp;

	// Explosion settings
	UPROPERTY(EditAnywhere, Category = "Explosion")
	bool bTriggerExplosion = false;

	UPROPERTY(EditAnywhere, Category = "Explosion")
	float ExplosionRadius = 300.f;

	UPROPERTY(EditAnywhere, Category = "Explosion")
	float ExplosionKnockbackForce = 2000.f;
	
	UPROPERTY(EditAnywhere, Category = "Explosion")
	TObjectPtr<UNiagaraSystem> ExplosionAsset;

	// Trail effect component
	TWeakObjectPtr<UNiagaraComponent> TrailEffect;
	
	// Trail effect asset
	UPROPERTY(EditAnywhere, Category = "Trail Effect")
	TObjectPtr<UNiagaraSystem> TrailEffectAsset;
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
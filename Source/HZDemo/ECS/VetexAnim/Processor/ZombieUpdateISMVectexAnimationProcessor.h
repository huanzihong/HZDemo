#pragma once
#include "MassUpdateISMProcessor.h"
#include "ZombieUpdateISMVectexAnimationProcessor.generated.h"
struct FZombieAnimationFragment;

USTRUCT(BlueprintType)
struct FZombieAnimState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
	float TimeOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
	float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
	float StartFrame = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
	float EndFrame = 0.0f; 
};
USTRUCT(BlueprintType)
struct FZombieInstancePlaybackData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
	FZombieAnimState CurrentState;
};

UCLASS()
class UZombieUpdateISMVectexAnimationProcessor : public UMassUpdateISMProcessor
{
	GENERATED_BODY()
public:
	UZombieUpdateISMVectexAnimationProcessor();
	
	static void UpdateISMVertexAnimation(FMassInstancedStaticMeshInfo& ISMInfo, FZombieAnimationFragment& AnimationData, const float LODSignificance, const float PrevLODSignificance, const int32 NumFloatsToPad = 0);

protected:

	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
};

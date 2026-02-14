#pragma once
#include "MassNavigationTypes.h"
#include "MassStateTreeTypes.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"
#include "EnemyStateEvaluator.generated.h"

struct FTransformFragment;

USTRUCT()
struct FEnemyStateEvaluatorInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Output)
	EEnemyState EnemyState = EEnemyState::None;

	UPROPERTY(EditAnywhere, Category=Output)
	FMassTargetLocation TargetLocation;

	UPROPERTY(EditAnywhere, Category=Output)
	bool bRePath = false;
	
	UPROPERTY(EditAnywhere, Category=Output)
	float DistanceToPlayer = 0.0f;
};

USTRUCT(meta = (DisplayName = "Enemy State Eval"))
struct FEnemyStateEvaluator : public FMassStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FEnemyStateEvaluatorInstanceData;
public:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	TStateTreeExternalDataHandle<FEnemyFragment> EnemyFragmentHandle;
	TStateTreeExternalDataHandle<FTransformFragment> EnemyTransformHandle;
};

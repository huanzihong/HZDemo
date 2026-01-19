#pragma once
#include "MassNavigationTypes.h"
#include "MassStateTreeTypes.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"
#include "EnemyStateEvaluator.generated.h"
USTRUCT()
struct FEnemyStateEvaluatorInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Output)
	EEnemyState EnemyState = EEnemyState::Other;

	UPROPERTY(EditAnywhere, Category=Output)
	FMassTargetLocation TargetLocation;

	UPROPERTY(EditAnywhere, Category=Output)
	bool bRePath = false;
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
};

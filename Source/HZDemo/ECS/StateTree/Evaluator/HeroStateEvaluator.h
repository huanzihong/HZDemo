#pragma once
#include "StateTreeEvaluatorBase.h"

#include "HeroStateEvaluator.generated.h"

USTRUCT()
struct HZDEMO_API FHeroStateEvaluatorInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Output)
	TObjectPtr<AActor> Player;
};

USTRUCT(meta = (DisplayName = "Hero State Eval"))
struct HZDEMO_API FHeroStateEvaluator : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FHeroStateEvaluatorInstanceData;
	//virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	//TStateTreeExternalDataHandle<FHeroFragment> EnemyFragmentHandle;
};

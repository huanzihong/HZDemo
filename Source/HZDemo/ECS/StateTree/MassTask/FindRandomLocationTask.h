#pragma once
#include "MassNavigationTypes.h"
#include "MassStateTreeTypes.h"
#include "FindRandomLocationTask.generated.h"
struct FTransformFragment;

USTRUCT()
struct HZDEMO_API FFindRandomLocationTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Input)
	float Range = 1000.f;
	
	UPROPERTY(EditAnywhere, Category=Output)
	FVector OutLocation;

	FFindRandomLocationTaskInstanceData() = default;
};

USTRUCT(meta = (DisplayName = "Find Random Location"))
struct HZDEMO_API FFindRandomLocation : public FMassStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FFindRandomLocationTaskInstanceData;

	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FFindRandomLocationTaskInstanceData::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

protected:
	TStateTreeExternalDataHandle<FTransformFragment> TransformHandle;

};
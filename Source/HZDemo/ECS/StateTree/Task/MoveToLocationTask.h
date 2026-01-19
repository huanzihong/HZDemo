#pragma once
#include "MassCommonFragments.h"
#include "MassNavigationFragments.h"
#include "MassStateTreeTypes.h"

#include "MoveToLocationTask.generated.h"

USTRUCT()
struct HZDEMO_API FMoveToLocationTaskData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Input)
	FVector Destination;

	UPROPERTY(EditAnywhere, Category=Output)
	FVector AgentLocation;

	FMoveToLocationTaskData() = default;
};

/**
 * Sets the target location that the entity should go to. Returns successful when the entity has reached the location
 */
USTRUCT(meta = (DisplayName = "Move To Location"))
struct HZDEMO_API FMoveToLocationTask : public FMassStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMoveToLocationTaskData;

	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FMoveToLocationTaskData::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
protected:
	TStateTreeExternalDataHandle<FMassMoveTargetFragment> MoveTargetHandle;
	TStateTreeExternalDataHandle<FTransformFragment> TransformHandle;
};
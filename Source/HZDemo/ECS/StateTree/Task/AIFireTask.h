#pragma once
#include "Tasks/StateTreeAITask.h"

#include "AIFireTask.generated.h"
class AAIController;
class UBulletSubsystem;

USTRUCT()
struct HZDEMO_API FAIFireTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Input)
	FVector FireLocation;
	
};

USTRUCT(meta = (DisplayName = "Fire Task", Category = "AI|Action"))
struct HZDEMO_API FAIFireTask : public FStateTreeAITaskBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FAIFireTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual bool Link(FStateTreeLinker& Linker) override;
protected:
	TStateTreeExternalDataHandle<UBulletSubsystem> BulletSubsystemHandle;
	
	TStateTreeExternalDataHandle<AActor> ActorHandle;
	TStateTreeExternalDataHandle<AAIController> AIControllerHandle;
};

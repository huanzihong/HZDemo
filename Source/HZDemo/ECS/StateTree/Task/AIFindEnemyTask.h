#pragma once
#include "MassEntitySubsystem.h"
#include "ECS/Enemy/Subsystem/HashGridSubsystem.h"
#include "Tasks/StateTreeAITask.h"

#include "AIFindEnemyTask.generated.h"
USTRUCT()
struct HZDEMO_API FAIFindEnemyTaskInstanceData
{
	GENERATED_BODY()

	
	UPROPERTY(EditAnywhere, Category = Parameter)
	float SerchRadius = 2000.f;
	
	UPROPERTY(EditAnywhere, Category=Output)
	bool bFindEnemy = false;
	
	UPROPERTY(EditAnywhere, Category=Output)
	FVector EnemyLocation{};
	
	
};

USTRUCT(meta = (DisplayName = "Find Enemy Task", Category = "AI|Action"))
struct HZDEMO_API FAIFindEnemyTask : public FStateTreeAITaskBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FAIFindEnemyTaskInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
	virtual bool Link(FStateTreeLinker& Linker) override;
	//virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	//virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
protected:
	TStateTreeExternalDataHandle<UHashGridSubsystem> HashGridSubsystemHandle;
	TStateTreeExternalDataHandle<UMassEntitySubsystem> EntitySubsystemHandle;
	TStateTreeExternalDataHandle<AActor> ActorHandle;
};

#pragma once
#include "MassEntitySubsystem.h"
#include "ECS/Enemy/Subsystem/HashGridSubsystem.h"
#include "Tasks/StateTreeAITask.h"
#include "Weapon/Weapon.h"

#include "AIFindWeaponTask.generated.h"
USTRUCT()
struct HZDEMO_API FAIFindWeaponTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Parameter)
	float SerchRadius = 2000.f;
	
	UPROPERTY(EditAnywhere, Category=Output)
	bool bFindWeapon;
	
	UPROPERTY(EditAnywhere, Category=Output)
	FVector WeaponLocation;
	
	UPROPERTY(EditAnywhere, Category=Output)
	TObjectPtr<AWeapon> WeaponActor;
};

USTRUCT(meta = (DisplayName = "FindWeaponTask", Category = "AI|Action"))
struct HZDEMO_API FAIFindWeaponTask : public FStateTreeAITaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAIFindWeaponTaskInstanceData;
	
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

#pragma once
#include "Tasks/StateTreeAITask.h"
#include "Weapon/Weapon.h"

#include "AIEquipWeaponTask.generated.h"

USTRUCT()
struct HZDEMO_API FAIEquipWeaponTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Input)
	TObjectPtr<AWeapon> WeaponActor;
};

USTRUCT(meta = (DisplayName = "Equip Weapon Task", Category = "AI|Action"))
struct HZDEMO_API FAIEquipWeaponTask : public FStateTreeAITaskBase
{
	GENERATED_BODY()
	using FInstanceDataType = FAIEquipWeaponTaskInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual bool Link(FStateTreeLinker& Linker) override;
	TStateTreeExternalDataHandle<AActor> ActorHandle;
};

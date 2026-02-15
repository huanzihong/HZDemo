#pragma once
#include "MassCommonFragments.h"
#include "MassNavigationFragments.h"
#include "MassStateTreeTypes.h"

#include "FindWeaponTask.generated.h"

class UHashGridSubsystem;
class UMassEntitySubsystem;

USTRUCT()
struct HZDEMO_API FFindWeaponTaskData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = Parameter)
	float SerchRadius = 2000.f;
	
	UPROPERTY(EditAnywhere, Category=Output)
	bool bFindWeapon;
	
	UPROPERTY(EditAnywhere, Category=Output)
	FMassTargetLocation WeaponLocation;
	
	FFindWeaponTaskData() = default;
};

/**
 * Sets the target location that the entity should go to. Returns successful when the entity has reached the location
 */
USTRUCT(meta = (DisplayName = "Find Weapon"))
struct HZDEMO_API FFindWeaponTask : public FMassStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FFindWeaponTaskData;

	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FFindWeaponTaskData::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
protected:
	TStateTreeExternalDataHandle<FTransformFragment> TransformFragmentHandle;
	TStateTreeExternalDataHandle<UHashGridSubsystem> HashGridSubsystemHandle;
	TStateTreeExternalDataHandle<UMassEntitySubsystem> EntitySubsystemHandle;

	
};
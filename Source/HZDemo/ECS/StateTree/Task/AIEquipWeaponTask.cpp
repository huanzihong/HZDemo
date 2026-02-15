#include "AIEquipWeaponTask.h"

#include "HZDemoCharacter.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Player/CombatComponent.h"

EStateTreeRunStatus FAIEquipWeaponTask::EnterState(FStateTreeExecutionContext& Context,
                                                   const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	if (InstanceData.WeaponActor)
	{
		AActor& AIActor = Context.GetExternalData(ActorHandle);
		
		
		if (UCombatComponent* CombatComponent = Cast<AHZDemoCharacter>(&AIActor)->GetCombatComponent())
		{
			CombatComponent->EquipWeapon(InstanceData.WeaponActor);
			return EStateTreeRunStatus::Succeeded;
		}
	}else
	{
		return EStateTreeRunStatus::Failed;
	}
	
	return EStateTreeRunStatus::Running;
}

bool FAIEquipWeaponTask::Link(FStateTreeLinker& Linker)
{
	
	Linker.LinkExternalData(ActorHandle);
	return FStateTreeAITaskBase::Link(Linker);
}

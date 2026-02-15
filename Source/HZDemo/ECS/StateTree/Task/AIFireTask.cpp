#include "AIFireTask.h"

#include "AIController.h"
#include "HZDemoCharacter.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "ECS/Bullet/BulletSubsystem.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Player/CombatComponent.h"
#include "Weapon/Weapon.h"

EStateTreeRunStatus FAIFireTask::EnterState(FStateTreeExecutionContext& Context,
                                            const FStateTreeTransitionResult& Transition) const
{
	
	AActor& AIActor = Context.GetExternalData(ActorHandle);
	UBulletSubsystem& BulletSubsystem = Context.GetExternalData(BulletSubsystemHandle);
	AAIController& AIController = Context.GetExternalData(AIControllerHandle);
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	AIController.SetFocalPoint(InstanceData.FireLocation);
	
	if(auto HZDemoCharacter = Cast<AHZDemoCharacter>(&AIActor))
	{
		if(auto CombatComponent = HZDemoCharacter->GetCombatComponent())
		{
			if (auto EquippedWeapon = CombatComponent->GetEquippedWeapon())
			{
				if(auto BulletConfig = EquippedWeapon->GetBulletConfig())
				{
					CombatComponent->SetAimming(true);
					//获取枪口位置
					const USkeletalMeshSocket* MuzzleFlashSocket = EquippedWeapon->GetWeaponMesh()->GetSocketByName("MuzzleFlash");
					if (MuzzleFlashSocket == nullptr) return EStateTreeRunStatus::Failed;

					const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(EquippedWeapon->GetWeaponMesh());
					FVector MuzzleLocation = SocketTransform.GetLocation();
					
					TOptional<FVector> TargetLocation;
					BulletSubsystem.SpawnBullet(BulletConfig, MuzzleLocation, (InstanceData.FireLocation - MuzzleLocation).GetSafeNormal(),TargetLocation);
					return EStateTreeRunStatus::Succeeded;
				}
			}
			
		}
	}
	
	return EStateTreeRunStatus::Failed;
	
}

bool FAIFireTask::Link(FStateTreeLinker& Linker)
{
	
	Linker.LinkExternalData(BulletSubsystemHandle);
	Linker.LinkExternalData(ActorHandle);
	Linker.LinkExternalData(AIControllerHandle);
	return FStateTreeAITaskBase::Link(Linker);
}

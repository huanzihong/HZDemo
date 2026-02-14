#include "EnemyStateEvaluator.h"

#include "MassCommonFragments.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Kismet/GameplayStatics.h"

bool FEnemyStateEvaluator::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(EnemyFragmentHandle);
	Linker.LinkExternalData(EnemyTransformHandle);
	return true;
}

void FEnemyStateEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	FEnemyFragment& EnemyFragment = Context.GetExternalData(EnemyFragmentHandle);
	FTransformFragment& EnemyTransform = Context.GetExternalData(EnemyTransformHandle);
	auto PlayerLocation = UGameplayStatics::GetPlayerPawn(Context.GetWorld(),0)->GetActorLocation();
	
	auto EnemyLocation = EnemyTransform.GetTransform().GetLocation();
	auto Direction = (EnemyLocation-PlayerLocation).GetSafeNormal();
	if(!InstanceData.TargetLocation.EndOfPathPosition.IsSet())
	{
		InstanceData.TargetLocation.EndOfPathPosition = PlayerLocation+Direction*EnemyFragment.CapsuleRadius*2;
		InstanceData.TargetLocation.EndOfPathIntent = EMassMovementAction::Stand;
		//DrawDebugSphere(Context.GetWorld(), InstanceData.TargetLocation.EndOfPathPosition.GetValue(), 10.0f, 32, FColor::Red, false, 5.0f);
	}
	float PlayerMoveDistance = FVector::Dist2D(InstanceData.TargetLocation.EndOfPathPosition.GetValue(),PlayerLocation);
	float EnemyDistanceToPlayer = FVector::Dist2D(InstanceData.TargetLocation.EndOfPathPosition.GetValue(),EnemyLocation);
	InstanceData.DistanceToPlayer = EnemyDistanceToPlayer;
	
	if(PlayerMoveDistance>100)
	{
		InstanceData.bRePath = true;
		InstanceData.TargetLocation.EndOfPathPosition = PlayerLocation+Direction*EnemyFragment.CapsuleRadius*2;
	}else
	{
		InstanceData.bRePath = false;
	}
	//DrawDebugSphere(Context.GetWorld(), InstanceData.TargetLocation.EndOfPathPosition.GetValue(), 10.0f, 32, FColor::Red, false, 5.0f);
	
	InstanceData.EnemyState = EnemyFragment.EnemyState;
}

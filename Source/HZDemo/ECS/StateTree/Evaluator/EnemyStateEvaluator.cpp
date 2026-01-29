#include "EnemyStateEvaluator.h"

#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Kismet/GameplayStatics.h"

bool FEnemyStateEvaluator::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(EnemyFragmentHandle);

	return true;
}

void FEnemyStateEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	FEnemyFragment& HitSubsystem = Context.GetExternalData(EnemyFragmentHandle);
	auto PlayerLocation = UGameplayStatics::GetPlayerPawn(Context.GetWorld(),0)->GetActorLocation();
	if(!InstanceData.TargetLocation.EndOfPathPosition.IsSet())
	{
		InstanceData.TargetLocation.EndOfPathPosition = PlayerLocation;
	}
	if(FVector::Dist2D(InstanceData.TargetLocation.EndOfPathPosition.GetValue(),PlayerLocation)>100)
	{
		InstanceData.bRePath = true;
		InstanceData.TargetLocation.EndOfPathPosition = PlayerLocation;
	}else
	{
		InstanceData.bRePath = false;
	}
	
	InstanceData.EnemyState = HitSubsystem.EnemyState;
}

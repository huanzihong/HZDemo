#include "HeroStateEvaluator.h"

#include "StateTreeExecutionContext.h"
#include "Kismet/GameplayStatics.h"


void FHeroStateEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.Player = UGameplayStatics::GetPlayerPawn(Context.GetWorld(),0);
}

void FHeroStateEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	//FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	//FEnemyFragment& HitSubsystem = Context.GetExternalData(EnemyFragmentHandle);
	//auto PlayerLocation = UGameplayStatics::GetPlayerPawn(Context.GetWorld(),0)->GetActorLocation();
	
	if (Context.GetInstanceData(*this).Player == nullptr)
	{
		Context.GetInstanceData(*this).Player = UGameplayStatics::GetPlayerPawn(Context.GetWorld(),0);
	}
	
}

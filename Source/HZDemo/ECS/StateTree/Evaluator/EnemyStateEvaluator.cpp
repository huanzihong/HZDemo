#include "EnemyStateEvaluator.h"

#include "MassCommonFragments.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Engine/World.h"

namespace
{
	bool FindNearestPlayerPawn(UWorld* World, const FVector& FromLocation, FVector& OutLocation)
	{
		if (!World)
		{
			return false;
		}

		float BestDistSq = TNumericLimits<float>::Max();
		bool bFound = false;

		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			const APlayerController* PC = It->Get();
			const APawn* Pawn = PC ? PC->GetPawn() : nullptr;
			if (!IsValid(Pawn))
			{
				continue;
			}

			const FVector PawnLocation = Pawn->GetActorLocation();
			const float DistSq = FVector::DistSquared2D(FromLocation, PawnLocation);
			if (DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				OutLocation = PawnLocation;
				bFound = true;
			}
		}

		return bFound;
	}
}

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

	auto EnemyLocation = EnemyTransform.GetTransform().GetLocation();
	FVector PlayerLocation = FVector::ZeroVector;
	const bool bHasPlayer = FindNearestPlayerPawn(Context.GetWorld(), EnemyLocation, PlayerLocation);
	if (!bHasPlayer)
	{
		InstanceData.bRePath = false;
		InstanceData.DistanceToPlayer = 0.0f;
		InstanceData.EnemyState = EnemyFragment.EnemyState;
		return;
	}

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

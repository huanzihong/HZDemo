#include "FMassReplicationNavPathHandler.h"
void FReplicatedAgentNavPathData::SetFromMoveTarget(const FMassMoveTargetFragment& MoveTargetFragment)
{
	MoveTargetCenter = MoveTargetFragment.Center;
	MoveTargetForward = MoveTargetFragment.Forward;
	DistanceToGoal = MoveTargetFragment.DistanceToGoal;
	SlackRadius = MoveTargetFragment.SlackRadius;
	DesiredSpeed = MoveTargetFragment.DesiredSpeed;
	IntentAtGoal = MoveTargetFragment.IntentAtGoal;
	Action = MoveTargetFragment.GetCurrentAction();
	ActionID = MoveTargetFragment.GetCurrentActionID();
	ActionServerStartTime = MoveTargetFragment.GetCurrentActionServerStartTime();
	bOffBoundaries = MoveTargetFragment.bOffBoundaries;
	bSteeringFallingBehind = MoveTargetFragment.bSteeringFallingBehind;
}

bool FReplicatedAgentNavPathData::UpdateFromMoveTarget(const FMassMoveTargetFragment& MoveTargetFragment)
{
	bool bChanged = false;

	auto UpdateVector = [&bChanged](FVector& InOutValue, const FVector& NewValue, const float Tolerance)
	{
		if (!InOutValue.Equals(NewValue, Tolerance))
		{
			InOutValue = NewValue;
			bChanged = true;
		}
	};

	auto UpdateFloat = [&bChanged](float& InOutValue, const float NewValue, const float Tolerance)
	{
		if (!FMath::IsNearlyEqual(InOutValue, NewValue, Tolerance))
		{
			InOutValue = NewValue;
			bChanged = true;
		}
	};

	UpdateVector(MoveTargetCenter, MoveTargetFragment.Center, 1.0f);
	UpdateVector(MoveTargetForward, MoveTargetFragment.Forward, 0.01f);
	UpdateFloat(DistanceToGoal, MoveTargetFragment.DistanceToGoal, 0.1f);
	UpdateFloat(SlackRadius, MoveTargetFragment.SlackRadius, 0.1f);

	if (DesiredSpeed != MoveTargetFragment.DesiredSpeed)
	{
		DesiredSpeed = MoveTargetFragment.DesiredSpeed;
		bChanged = true;
	}

	if (IntentAtGoal != MoveTargetFragment.IntentAtGoal)
	{
		IntentAtGoal = MoveTargetFragment.IntentAtGoal;
		bChanged = true;
	}

	if (Action != MoveTargetFragment.GetCurrentAction())
	{
		Action = MoveTargetFragment.GetCurrentAction();
		bChanged = true;
	}

	if (ActionID != MoveTargetFragment.GetCurrentActionID())
	{
		ActionID = MoveTargetFragment.GetCurrentActionID();
		bChanged = true;
	}

	const double NewActionServerStartTime = MoveTargetFragment.GetCurrentActionServerStartTime();
	if (!FMath::IsNearlyEqual(ActionServerStartTime, NewActionServerStartTime, 0.001))
	{
		ActionServerStartTime = NewActionServerStartTime;
		bChanged = true;
	}

	if (bOffBoundaries != MoveTargetFragment.bOffBoundaries)
	{
		bOffBoundaries = MoveTargetFragment.bOffBoundaries;
		bChanged = true;
	}

	if (bSteeringFallingBehind != MoveTargetFragment.bSteeringFallingBehind)
	{
		bSteeringFallingBehind = MoveTargetFragment.bSteeringFallingBehind;
		bChanged = true;
	}

	return bChanged;
}



void FMassReplicationProcessorNavPathHandler::AddRequirements(FMassEntityQuery& InQuery)
{
	InQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
}

void FMassReplicationProcessorNavPathHandler::CacheFragmentViews(FMassExecutionContext& ExecContext)
{
	MoveTargetList = ExecContext.GetMutableFragmentView<FMassMoveTargetFragment>();
	
}

void FMassReplicationProcessorNavPathHandler::AddEntity(const int32 EntityIdx,
	FReplicatedAgentNavPathData& InOUtReplicatedPathData) const
{
	const FMassMoveTargetFragment& MoveTargetFragment = MoveTargetList[EntityIdx];
	InOUtReplicatedPathData.SetFromMoveTarget(MoveTargetFragment);
	
}

FVector FMassReplicationProcessorNavPathHandler::GetMoveTargetCenter(const int32 EntityIdx) const
{
	return MoveTargetList[EntityIdx].Center;
}

const FMassMoveTargetFragment& FMassReplicationProcessorNavPathHandler::GetMoveTargetFragment(const int32 EntityIdx) const
{
	return MoveTargetList[EntityIdx];
}


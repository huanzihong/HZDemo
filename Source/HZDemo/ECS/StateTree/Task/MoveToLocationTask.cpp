// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToLocationTask.h"

#include "MassNavigationFragments.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

bool FMoveToLocationTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(MoveTargetHandle);
	Linker.LinkExternalData(TransformHandle);
	return FMassStateTreeTaskBase::Link(Linker);
}

EStateTreeRunStatus FMoveToLocationTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const FVector& Destination = InstanceData.Destination;
	auto& TransformFragment = Context.GetExternalData(TransformHandle);

	// Create movement action
	auto& MoveTarget = Context.GetExternalData(MoveTargetHandle);
	MoveTarget.Center = Destination;
	MoveTarget.SlackRadius = 50.f;
	MoveTarget.Forward = (Destination - TransformFragment.GetTransform().GetLocation()).GetSafeNormal();
	MoveTarget.DistanceToGoal = FVector::Dist(Destination, TransformFragment.GetTransform().GetLocation());
	MoveTarget.IntentAtGoal = EMassMovementAction::Stand;
	MoveTarget.CreateNewAction(EMassMovementAction::Move, *Context.GetWorld());
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FMoveToLocationTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const auto& TransformFragment = Context.GetExternalData(TransformHandle);
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	auto& MoveTarget = Context.GetExternalData(MoveTargetHandle);
	InstanceData.AgentLocation = TransformFragment.GetTransform().GetLocation();
	MoveTarget.DistanceToGoal = FVector::Dist(MoveTarget.Center, InstanceData.AgentLocation);
	MoveTarget.Forward = (MoveTarget.Center - TransformFragment.GetTransform().GetLocation()).GetSafeNormal();

	if (MoveTarget.DistanceToGoal <= MoveTarget.SlackRadius)
	{
		MoveTarget.CreateNewAction(MoveTarget.IntentAtGoal, *Context.GetWorld());
		return EStateTreeRunStatus::Succeeded;
	}
	
	return EStateTreeRunStatus::Running;
}

void FMoveToLocationTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	auto& MoveTarget = Context.GetExternalData(MoveTargetHandle);
	MoveTarget.CreateNewAction(EMassMovementAction::Stand, *Context.GetWorld());
}

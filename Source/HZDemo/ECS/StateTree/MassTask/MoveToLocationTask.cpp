// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToLocationTask.h"

#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "MassStateTreeExecutionContext.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

bool FMoveToLocationTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(MoveTargetHandle);
	Linker.LinkExternalData(TransformHandle);
	Linker.LinkExternalData(MovementParamsHandle);
	Linker.LinkExternalData(DesiredMovementHandle);
	return FMassStateTreeTaskBase::Link(Linker);
}

EStateTreeRunStatus FMoveToLocationTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FMassStateTreeExecutionContext& MassContext = static_cast<FMassStateTreeExecutionContext&>(Context);
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const FVector& Destination = InstanceData.Destination;
	auto& TransformFragment = Context.GetExternalData(TransformHandle);

	// Create movement action
	auto& MoveTarget = Context.GetExternalData(MoveTargetHandle);
	MoveTarget.Center = Destination;
	//DrawDebugSphere(Context.GetWorld(), Destination, 50.f, 12, FColor::Red, false, 5.f);
	
	const FMassMovementParameters& MovementParams = Context.GetExternalData(MovementParamsHandle);
	/*float DesiredSpeed = FMath::Min(
		MovementParams.GenerateDesiredSpeed(InstanceData.MovementStyle, MassContext.GetEntity().Index) * InstanceData.SpeedScale,
		MovementParams.MaxSpeed);

	// Apply DesiredMaxSpeedOverride
	const FMassDesiredMovementFragment& DesiredMovementFragment = Context.GetExternalData(DesiredMovementHandle);
	DesiredSpeed = FMath::Min(DesiredSpeed, DesiredMovementFragment.DesiredMaxSpeedOverride);*/

	MoveTarget.DesiredSpeed.Set(InstanceData.DesiredSpeed);
	
	MoveTarget.SlackRadius = 0.f;
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
	//DrawDebugSphere(Context.GetWorld(), MoveTarget.Center, 50.f, 12, FColor::Blue, false, 5.f);
	//DrawDebugSphere(Context.GetWorld(), InstanceData.AgentLocation, 50.f, 12, FColor::Green, false, 5.f);
	MoveTarget.Forward = (MoveTarget.Center - TransformFragment.GetTransform().GetLocation()).GetSafeNormal();
	// Update MoveTarget
	
	if (MoveTarget.DistanceToGoal <= 100)
	{
		//MoveTarget.DistanceToGoal = 0;
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

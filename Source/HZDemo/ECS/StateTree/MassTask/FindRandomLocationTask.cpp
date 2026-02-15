#include "FindRandomLocationTask.h"

#include "MassCommonFragments.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Kismet/GameplayStatics.h"

EStateTreeRunStatus FFindRandomLocation::EnterState(FStateTreeExecutionContext& Context,
                                                        const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const float Range = InstanceData.Range;
	FTransformFragment& Transform = Context.GetExternalData(TransformHandle);
	const auto NewOffset = FVector(FMath::RandRange(Range/2*-1, Range/2), FMath::RandRange(Range/2*-1, Range/2), 0.f);;
	
	InstanceData.OutLocation = Transform.GetTransform().GetLocation()+NewOffset;
	
	return EStateTreeRunStatus::Running;
}

bool FFindRandomLocation::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(TransformHandle);
	
	return FMassStateTreeTaskBase::Link(Linker);
}

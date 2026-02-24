#include "FMassReplicationNavPathHandler.h"



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
	InOUtReplicatedPathData.SetMoveTargetCenter(MoveTargetFragment.Center);
	
}

FVector FMassReplicationProcessorNavPathHandler::GetMoveTargetCenter(const int32 EntityIdx) const
{
	return MoveTargetList[EntityIdx].Center;
}

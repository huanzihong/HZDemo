#include "ZombieUpdateISMVectexAnimationProcessor.h"

#include "MassRepresentationFragments.h"
#include "MassRepresentationSubsystem.h"
#include "MassRepresentationTypes.h"
#include "ECS/StateTree/Task/MoveToLocationTask.h"
#include "ECS/VetexAnim/Traits/ZombieAnimationFragment.h"

UZombieUpdateISMVectexAnimationProcessor::UZombieUpdateISMVectexAnimationProcessor()
{
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Tasks);
}

void UZombieUpdateISMVectexAnimationProcessor::UpdateISMVertexAnimation(FMassInstancedStaticMeshInfo& ISMInfo,
	FZombieAnimationFragment& AnimationData, const float LODSignificance, const float PrevLODSignificance,
	const int32 NumFloatsToPad)
{
	//FZombieInstancePlaybackData InstanceData;
	const FAnimToTextureAnimInfo& AnimInfo = AnimationData.AnimToTextureData->Animations[AnimationData.AnimationStateIndex];
	TArray<float> CustomData;
	CustomData.Add(AnimInfo.StartFrame);
	CustomData.Add(AnimInfo.EndFrame);
	CustomData.Add(0);
	CustomData.Add(1);
	/*InstanceData.CurrentState.StartFrame = AnimInfo.StartFrame;
	InstanceData.CurrentState.EndFrame = AnimInfo.EndFrame;
	InstanceData.CurrentState.PlayRate = 1.f;*/
	
	ISMInfo.AddBatchedCustomDataFloats(CustomData,LODSignificance,PrevLODSignificance);
}

void UZombieUpdateISMVectexAnimationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	Super::ConfigureQueries(EntityManager);
	EntityQuery.AddRequirement<FZombieAnimationFragment>(EMassFragmentAccess::ReadWrite);
}

void UZombieUpdateISMVectexAnimationProcessor::Execute(FMassEntityManager& EntityManager,
	FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		UMassRepresentationSubsystem* RepresentationSubsystem = Context.GetSharedFragment<FMassRepresentationSubsystemSharedFragment>().RepresentationSubsystem;
		check(RepresentationSubsystem);
		FMassInstancedStaticMeshInfoArrayView ISMInfo = RepresentationSubsystem->GetMutableInstancedStaticMeshInfos();

		TConstArrayView<FTransformFragment> TransformList = Context.GetFragmentView<FTransformFragment>();
		TArrayView<FMassRepresentationFragment> RepresentationList = Context.GetMutableFragmentView<FMassRepresentationFragment>();
		TConstArrayView<FMassRepresentationLODFragment> RepresentationLODList = Context.GetFragmentView<FMassRepresentationLODFragment>();
		TArrayView<FZombieAnimationFragment> AnimationDataList = Context.GetMutableFragmentView<FZombieAnimationFragment>();

		for (FMassExecutionContext::FEntityIterator EntityIt = Context.CreateEntityIterator(); EntityIt; ++EntityIt)
		{
			const FTransformFragment& TransformFragment = TransformList[EntityIt];
			const FMassRepresentationLODFragment& RepresentationLOD = RepresentationLODList[EntityIt];
			FMassRepresentationFragment& Representation = RepresentationList[EntityIt];
			FZombieAnimationFragment& AnimationData = AnimationDataList[EntityIt];

			if (Representation.CurrentRepresentation == EMassRepresentationType::StaticMeshInstance)
			{
				/*UpdateISMTransform(Context.GetEntity(EntityIt), ISMInfo[Representation.StaticMeshDescHandle.ToIndex()]
					, TransformFragment.GetTransform(), Representation.PrevTransform, RepresentationLOD.LODSignificance, Representation.PrevLODSignificance);*/
				UpdateISMVertexAnimation(ISMInfo[Representation.StaticMeshDescHandle.ToIndex()], AnimationData, RepresentationLOD.LODSignificance, Representation.PrevLODSignificance);
			}
			Representation.PrevTransform = TransformFragment.GetTransform();
			Representation.PrevLODSignificance = RepresentationLOD.LODSignificance;
		}
	});
}



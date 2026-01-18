#include "EnemyBeHitProcessor.h"

#include "MassCommonFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassSignalSubsystem.h"
#include "ECS/Enemy/Traits/BeHitTags.h"
#include "ECS/Enemy/Traits/KnockbackFragment.h"

void UKnockSignalProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	
}

void UKnockSignalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& EntityManager)
{
	Super::InitializeInternal(Owner, EntityManager);
	UMassSignalSubsystem* SignalSubsystem = UWorld::GetSubsystem<UMassSignalSubsystem>(Owner.GetWorld());
	SubscribeToSignal(*SignalSubsystem, FName(TEXT("Knock")));
}

void UKnockSignalProcessor::SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context,
	FMassSignalNameLookup& EntitySignals)
{
	EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
	{
		const int32 NumEntities = Context.GetNumEntities();
		for (const FMassEntityHandle EntityHandle : Context.GetEntities())
		{
			Context.GetEntityManagerChecked().AddTagToEntity(EntityHandle,FKnockTag::StaticStruct());
		}
	});
}

UKnockSignalProcessor::UKnockSignalProcessor():EntityQuery(*this)
{
}

void UKnockBackProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddTagRequirement<FKnockTag>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FKnockbackFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassDesiredMovementFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassForceFragment>(EMassFragmentAccess::ReadWrite);
}

void UKnockBackProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	double CurrentTime = GetWorld()->GetTimeSeconds();
	EntityQuery.ForEachEntityChunk(Context, [this,CurrentTime](FMassExecutionContext& Context)
	{
		
		const TArrayView<FTransformFragment> TransformList = Context.GetMutableFragmentView<FTransformFragment>();
		const TArrayView<FMassVelocityFragment> VelocityList = Context.GetMutableFragmentView<FMassVelocityFragment>();
		const TArrayView<FKnockbackFragment> KnockbackList = Context.GetMutableFragmentView<FKnockbackFragment>();
		const TArrayView<FMassDesiredMovementFragment> MovementList = Context.GetMutableFragmentView<FMassDesiredMovementFragment>();
		const TArrayView<FMassForceFragment> MassForceFragment = Context.GetMutableFragmentView<FMassForceFragment>();
		const int32 NumEntities = Context.GetNumEntities();
		
		for (int32 i = 0; i < NumEntities; ++i)
		{
			FKnockbackFragment& Knockback = KnockbackList[i];
			FMassDesiredMovementFragment& DesiredMovementFragment = MovementList[i];
                
			float ElapsedTime = CurrentTime - Knockback.StartTime;
                
			if (ElapsedTime < Knockback.Duration)
			{
				// 计算击退衰减（使用二次衰减曲线）
				float Alpha = ElapsedTime / Knockback.Duration;
				float Attenuation = 1.0f - FMath::Square(Alpha);
                    
				// 应用击退速度
				DesiredMovementFragment.DesiredVelocity = Knockback.OriginalVelocity + Knockback.Direction * Knockback.Force * Attenuation+FVector{0,0,1000};
			}
			else
			{
				// 击退结束，恢复原始速度
				DesiredMovementFragment.DesiredVelocity = FVector::ZeroVector;
				
				// 移除击退标签
				Context.Defer().RemoveTag<FKnockTag>(Context.GetEntity(i));
			}
		}
	});
}

UKnockBackProcessor::UKnockBackProcessor():EntityQuery(*this)
{
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::ApplyForces);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Movement);
}





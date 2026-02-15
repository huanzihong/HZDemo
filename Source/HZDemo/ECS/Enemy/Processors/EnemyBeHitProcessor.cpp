#include "EnemyBeHitProcessor.h"

#include "MassCommonFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassSignalSubsystem.h"
#include "Anim/HZZombieAnimInstance.h"
#include "ECS/Enemy/Traits/BeHitTags.h"
#include "ECS/Enemy/Traits/KnockbackFragment.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"
#include "Player/HZZombie.h"
#include "MassActorSubsystem.h"

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
	EntityQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FEnemyFragment>(EMassFragmentAccess::ReadWrite);
}

void UKnockBackProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	double CurrentTime = GetWorld()->GetTimeSeconds();
	EntityQuery.ForEachEntityChunk(Context, [this,CurrentTime](FMassExecutionContext& Context)
	{
		const TArrayView<FKnockbackFragment> KnockbackList = Context.GetMutableFragmentView<FKnockbackFragment>();
		const TArrayView<FMassDesiredMovementFragment> MovementList = Context.GetMutableFragmentView<FMassDesiredMovementFragment>();
		const TArrayView<FMassActorFragment> ActorList = Context.GetMutableFragmentView<FMassActorFragment>();
		TArrayView<FEnemyFragment> EnemyList = Context.GetMutableFragmentView<FEnemyFragment>();
		const int32 NumEntities = Context.GetNumEntities();

		for (int32 i = 0; i < NumEntities; ++i)
		{
			FKnockbackFragment& Knockback = KnockbackList[i];
			FMassDesiredMovementFragment& DesiredMovementFragment = MovementList[i];
			const FMassActorFragment& Actor = ActorList[i];
			FEnemyFragment& Enemy = EnemyList[i];
			UHZZombieAnimInstance* ZombieAnimInstance = nullptr;
			if (auto ZombieActor = Actor.Get())
			{
				if (const ACharacter* Character = Cast<ACharacter>(ZombieActor))
				{
					
					if (auto SkeletalMeshComponent = Character->GetMesh())
					{
						ZombieAnimInstance = Cast<UHZZombieAnimInstance>(SkeletalMeshComponent->GetAnimInstance());
					}
				}
			}
			float ElapsedTime = CurrentTime - Knockback.StartTime;

			if (ElapsedTime < Knockback.Duration)
			{
				if (Enemy.EnemyState == EEnemyState::ChasePlayer)
				{
					Context.Defer().RemoveTag<FChasePlayerTag>(Context.GetEntity(i));
				}
				
				if (Enemy.EnemyState != EEnemyState::Knock)
				{
					Enemy.EnemyState = EEnemyState::Knock;
				}
				// 计算击退衰减（使用二次衰减曲线）
				float Alpha = ElapsedTime / Knockback.Duration;
				float Attenuation = 1.0f - FMath::Square(Alpha);

				// 应用击退速度
				DesiredMovementFragment.DesiredVelocity = (Knockback.Direction*1.5+FVector{0,0,4}) * Knockback.Force * Attenuation;
				if (ZombieAnimInstance)
				{
					ZombieAnimInstance->SetKnockedBack(true);
				}

			}
			else
			{
				// 击退结束，恢复原始速度
				DesiredMovementFragment.DesiredVelocity = FVector::ZeroVector;
				
				// 移除击退标签
				Context.Defer().RemoveTag<FKnockTag>(Context.GetEntity(i));
				Context.Defer().DestroyEntity(Context.GetEntity(i));
				if (ZombieAnimInstance)
				{
					ZombieAnimInstance->SetKnockedBack(false);
				}
				
				if (Enemy.EnemyState == EEnemyState::Knock)
				{
					Enemy.EnemyState = EEnemyState::None;
				}
			}
		}
	});
}

UKnockBackProcessor::UKnockBackProcessor():EntityQuery(*this)
{
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::ApplyForces);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Movement);
}





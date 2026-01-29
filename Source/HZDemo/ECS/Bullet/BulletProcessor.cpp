// Fill out your copyright notice in the Description page of Project Settings.
#include "BulletProcessor.h"

#include "BulletFragment.h"
#include "BulletSubsystem.h"
#include "MassCommonFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassSignalSubsystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "DrawDebugHelpers.h"
#include "ECS/Enemy/Subsystem/EnemyHashGridSubsystem.h"
#include "ECS/Enemy/Traits/BeHitTags.h"
#include "ECS/Enemy/Traits/KnockbackFragment.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"

UBulletInitializerProcessor::UBulletInitializerProcessor()
	: EntityQuery(*this)
{
}

void UBulletInitializerProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddTagRequirement<FBulletTag>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FBulletFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<UMassSignalSubsystem>(EMassFragmentAccess::ReadWrite);
}

void UBulletInitializerProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& EntityManager)
{
	Super::InitializeInternal(Owner, EntityManager);

	UMassSignalSubsystem* SignalSubsystem = UWorld::GetSubsystem<UMassSignalSubsystem>(Owner.GetWorld());
	SubscribeToSignal(*SignalSubsystem, BulletHell::Signals::BulletSpawned);
}

void UBulletInitializerProcessor::SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context,
                                                 FMassSignalNameLookup& EntitySignals)
{
	EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
	{
		auto SignalSubsystem = Context.GetMutableSubsystem<UMassSignalSubsystem>();
		auto BulletFragments = Context.GetMutableFragmentView<FBulletFragment>();
		auto VelocityFragments = Context.GetMutableFragmentView<FMassVelocityFragment>();
		auto TransformFragments = Context.GetMutableFragmentView<FTransformFragment>();

		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& BulletFragment = BulletFragments[EntityIdx];
			auto& VelocityFragment = VelocityFragments[EntityIdx];
			auto& TransformFragment = TransformFragments[EntityIdx];

			VelocityFragment.Value = BulletFragment.Direction.GetSafeNormal() * BulletFragment.Speed;
			TransformFragment.GetMutableTransform().SetLocation(BulletFragment.SpawnLocation);

			// Spawn trail effect (will be assigned in BulletSubsystem::SpawnBullet)
			// Trail effect is spawned externally to avoid loading assets in processor

			SignalSubsystem->DelaySignalEntityDeferred(Context, BulletHell::Signals::BulletDestroy, Context.GetEntity(EntityIdx), BulletFragment.Lifetime);
		}
	});
}

void UBulletDestroyerProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddTagRequirement<FBulletTag>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FBulletFragment>(EMassFragmentAccess::ReadWrite);
}

void UBulletDestroyerProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& EntityManager)
{
	Super::InitializeInternal(Owner, EntityManager);

	UMassSignalSubsystem* SignalSubsystem = UWorld::GetSubsystem<UMassSignalSubsystem>(Owner.GetWorld());
	SubscribeToSignal(*SignalSubsystem, BulletHell::Signals::BulletDestroy);
}

void UBulletDestroyerProcessor::SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context,
	FMassSignalNameLookup& EntitySignals)
{
	EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
	{
		auto BulletFragments = Context.GetMutableFragmentView<FBulletFragment>();
		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& BulletFragment = BulletFragments[EntityIdx];

			// Clean up trail effect
			if (BulletFragment.TrailEffect.IsValid())
			{
				// Deactivate and let it auto-destroy after particles die
				BulletFragment.TrailEffect->Deactivate();
				BulletFragment.TrailEffect->SetAutoDestroy(true);
			}

			Context.Defer().DestroyEntity(Context.GetEntity(EntityIdx));
		}
	});
}

UBulletDestroyerProcessor::UBulletDestroyerProcessor()
	: EntityQuery(*this)
{
	
}

UBulletCollisionProcessor::UBulletCollisionProcessor()
	: EntityQuery(*this)
{
}

void UBulletCollisionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddTagRequirement<FBulletTag>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FBulletFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<UEnemyHashGridSubsystem>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<UMassSignalSubsystem>(EMassFragmentAccess::ReadWrite);
}

void UBulletCollisionProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [this, &EntityManager](FMassExecutionContext& Context)
	{
		auto HashGridSubsystem = Context.GetSubsystem<UEnemyHashGridSubsystem>();
		auto TransformFragments = Context.GetFragmentView<FTransformFragment>();
		auto BulletFragments = Context.GetFragmentView<FBulletFragment>();
		const int32 NumEntities = Context.GetNumEntities();
		
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& TransformFragment = TransformFragments[EntityIdx];
			auto Location = TransformFragment.GetTransform().GetLocation();
			auto BulletFragment = BulletFragments[EntityIdx];
			TArray<FMassEntityHandle> Entities;
			HashGridSubsystem->GetHashGrid().Query(FBox::BuildAABB(Location, FVector(50.f)), Entities);

			// 精确胶囊体求交
			Entities = Entities.FilterByPredicate([&Location, &EntityManager](const FMassEntityHandle& Entity)
			{
				const FTransformFragment* TransformFrag = EntityManager.GetFragmentDataPtr<FTransformFragment>(Entity);
				const FEnemyFragment* EnemyFrag = EntityManager.GetFragmentDataPtr<FEnemyFragment>(Entity);

				if (!TransformFrag || !EnemyFrag) return false;

				// 胶囊体参数
				FVector EnemyLocation = TransformFrag->GetTransform().GetLocation()+FVector{0,0,90};
				float CapsuleRadius = EnemyFrag->CapsuleRadius;
				float CapsuleHalfHeight = EnemyFrag->CapsuleHalfHeight;

				// 计算胶囊体的顶部和底部中心点
				FVector CapsuleTop = EnemyLocation + FVector(0, 0, CapsuleHalfHeight);
				FVector CapsuleBottom = EnemyLocation - FVector(0, 0, CapsuleHalfHeight);

				// 计算点到胶囊体线段的最近点
				FVector ClosestPoint = FMath::ClosestPointOnSegment(Location, CapsuleBottom, CapsuleTop);

				// 判断距离是否小于半径
				float DistanceSquared = FVector::DistSquared(Location, ClosestPoint);
				return DistanceSquared <= (CapsuleRadius * CapsuleRadius);
			});

			// Check if bullet hit any enemy
			if(Entities.Num() > 0)
			{
				// Apply knockback to hit enemies
				/*for(auto Entity: Entities)
				{
					auto KnockbackFragment = EntityManager.GetFragmentDataPtr<FKnockbackFragment>(Entity);
					auto EntityLocation = EntityManager.GetFragmentDataPtr<FTransformFragment>(Entity)->GetTransform().GetLocation();
					KnockbackFragment->Direction = (EntityLocation-Location).GetSafeNormal();
					KnockbackFragment->StartTime = GetWorld()->GetTimeSeconds();
					KnockbackFragment->Force = BulletFragment.DestructForce;
					Context.Defer().AddTag<FKnockTag>(Entity);
				}*/

				// Handle explosion if enabled
				if (BulletFragment.bTriggerExplosion)
				{
#if ENABLE_DRAW_DEBUG
					// Draw explosion radius
					DrawDebugSphere(
						GetWorld(),
						Location,
						BulletFragment.ExplosionRadius,
						32,
						FColor::Red,
						false,
						2.0f,
						0,
						2.0f
					);
#endif

					TArray<FMassEntityHandle> ExplosionEntities;
					HashGridSubsystem->GetHashGrid().Query(
						FBox::BuildAABB(Location, FVector(BulletFragment.ExplosionRadius)),
						ExplosionEntities
					);

					// Filter enemies in explosion radius
					for (auto Entity : ExplosionEntities)
					{
						const FTransformFragment* TransformFrag = EntityManager.GetFragmentDataPtr<FTransformFragment>(Entity);
						if (!TransformFrag) continue;

						FVector EnemyLocation = TransformFrag->GetTransform().GetLocation();
						float Distance = FVector::Dist(Location, EnemyLocation);

						// Apply explosion knockback with falloff
						if (Distance <= BulletFragment.ExplosionRadius)
						{
							auto KnockbackFragment = EntityManager.GetFragmentDataPtr<FKnockbackFragment>(Entity);
							if (KnockbackFragment)
							{
								float FalloffRatio = 1.0f - (Distance / BulletFragment.ExplosionRadius);
								KnockbackFragment->Direction = (EnemyLocation - Location).GetSafeNormal();
								KnockbackFragment->StartTime = GetWorld()->GetTimeSeconds();
								KnockbackFragment->Force = BulletFragment.ExplosionKnockbackForce * FalloffRatio;
								Context.Defer().AddTag<FKnockTag>(Entity);

#if ENABLE_DRAW_DEBUG
								// Draw line from explosion center to affected enemy
								DrawDebugLine(
									GetWorld(),
									Location,
									EnemyLocation,
									FColor::Orange,
									false,
									2.0f,
									0,
									1.0f
								);
#endif
							}
						}
					}
				}

				// Clean up trail effect before destroying bullet
				auto& BulletFrag = BulletFragments[EntityIdx];
				if (BulletFrag.TrailEffect.IsValid())
				{
					// Deactivate and let it auto-destroy after particles die
					BulletFrag.TrailEffect->Deactivate();
					BulletFrag.TrailEffect->SetAutoDestroy(true);
				}

				Context.Defer().DestroyEntity(Context.GetEntity(EntityIdx));
			}

		}
	});
}

// Trail update processor implementation
UBulletTrailUpdateProcessor::UBulletTrailUpdateProcessor()
	: EntityQuery(*this)
{
}

void UBulletTrailUpdateProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddTagRequirement<FBulletTag>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FBulletFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
}

void UBulletTrailUpdateProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
	{
		auto BulletFragments = Context.GetFragmentView<FBulletFragment>();
		auto TransformFragments = Context.GetFragmentView<FTransformFragment>();
		const int32 NumEntities = Context.GetNumEntities();

		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& BulletFragment = BulletFragments[EntityIdx];
			auto& TransformFragment = TransformFragments[EntityIdx];

			// Update trail effect position
			if (BulletFragment.TrailEffect.IsValid())
			{
				FTransform CurrentTransform = TransformFragment.GetTransform();
				BulletFragment.TrailEffect->SetWorldLocation(CurrentTransform.GetLocation());
				BulletFragment.TrailEffect->SetWorldRotation(BulletFragment.Direction.Rotation());
			}
		}
	});
}

// Fill out your copyright notice in the Description page of Project Settings.
#include "BulletProcessor.h"

#include "BulletFragment.h"
#include "BulletSubsystem.h"
#include "MassCommonFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassSignalSubsystem.h"
#include "ECS/Enemy/Subsystem/EnemyHashGridSubsystem.h"
#include "ECS/Enemy/Traits/BeHitTags.h"
#include "ECS/Enemy/Traits/KnockbackFragment.h"

UBulletInitializerProcessor::UBulletInitializerProcessor()
	: EntityQuery(*this)
{
}

void UBulletInitializerProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddTagRequirement<FBulletTag>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FBulletFragment>(EMassFragmentAccess::ReadOnly);
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
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this](FMassExecutionContext& Context)
	{
		auto SignalSubsystem = Context.GetMutableSubsystem<UMassSignalSubsystem>();
		auto BulletFragments = Context.GetFragmentView<FBulletFragment>();
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

			SignalSubsystem->DelaySignalEntityDeferred(Context, BulletHell::Signals::BulletDestroy, Context.GetEntity(EntityIdx), BulletFragment.Lifetime);
		}
	});
}

void UBulletDestroyerProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddTagRequirement<FBulletTag>(EMassFragmentPresence::All);
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
		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
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

			Entities = Entities.FilterByPredicate([&Location, &EntityManager](const FMassEntityHandle& Entity)
			{
				auto EntityLocation = EntityManager.GetFragmentDataPtr<FTransformFragment>(Entity)->GetTransform().GetLocation();
				//DrawDebugPoint(EntityManager.GetWorld(), EntityLocation, 50.f, FColor::Yellow, true);
				//DrawDebugPoint(EntityManager.GetWorld(), Location, 50.f, FColor::Blue, true);
				return FVector::Dist(Location, EntityLocation) <= 500.f;
			});
			
			for(auto Entity: Entities)
			{
				auto KnockbackFragment = EntityManager.GetFragmentDataPtr<FKnockbackFragment>(Entity);
				auto EntityLocation = EntityManager.GetFragmentDataPtr<FTransformFragment>(Entity)->GetTransform().GetLocation();
				auto VelocityFragment = EntityManager.GetFragmentDataPtr<FMassDesiredMovementFragment>(Entity);
				KnockbackFragment->Direction = (EntityLocation-Location).GetSafeNormal();
				KnockbackFragment->StartTime = GetWorld()->GetTimeSeconds();
				KnockbackFragment->Force = BulletFragment.DestructForce;
				KnockbackFragment->OriginalVelocity = VelocityFragment->DesiredVelocity;
				Context.Defer().AddTag<FKnockTag>(Entity);
			}
			/*if (Entities.Num() > 0)
			{
				Entities.Add(Context.GetEntity(EntityIdx));
				Context.Defer().DestroyEntities(Entities); // Delete bullet as well
			}*/

			if(Entities.Num() > 0)
			{
				Context.Defer().DestroyEntity(Context.GetEntity(EntityIdx));
			}
			
		}
	});
}

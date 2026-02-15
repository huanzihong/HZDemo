#pragma once
#include "MassObserverProcessor.h"
#include "MassProcessor.h"
#include "ZombieAnimProcessor.generated.h"

UCLASS()
class UZombieAnimInitializerProcessor : public UMassObserverProcessor
{
	GENERATED_BODY()
public:
	UZombieAnimInitializerProcessor();
protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery EntityQuery;
};
UCLASS()
class UZombieAnimProcessor : public UMassProcessor 
{
	GENERATED_BODY()
public:
	UZombieAnimProcessor();
protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery AnimationEntityQuery;
};

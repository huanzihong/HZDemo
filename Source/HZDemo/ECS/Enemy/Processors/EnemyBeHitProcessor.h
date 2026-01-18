#pragma once
#include "MassSignalProcessorBase.h"

#include "EnemyBeHitProcessor.generated.h"

UCLASS()
class UKnockSignalProcessor : public UMassSignalProcessorBase
{
	GENERATED_BODY()

public:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context, FMassSignalNameLookup& EntitySignals) override;

	UKnockSignalProcessor();
	FMassEntityQuery EntityQuery;
	
};

UCLASS()
class UKnockBackProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
	
	UKnockBackProcessor();
	FMassEntityQuery EntityQuery;
	
};




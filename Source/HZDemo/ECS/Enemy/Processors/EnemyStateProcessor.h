#pragma once
#include "MassProcessor.h"
#include "EnemyStateProcessor.generated.h"
UCLASS()
class HZDEMO_API UEnemyStateProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UEnemyStateProcessor();

protected:
	
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

	
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	
	FMassEntityQuery EntityQuery;
};
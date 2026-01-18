#pragma once
#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "EnemyMoveProcessor.generated.h"
UCLASS()
class HZDEMO_API UEnemyMoveToPlayerProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UEnemyMoveToPlayerProcessor();

protected:
	
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

	
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	
	FMassEntityQuery EntityQuery;
};

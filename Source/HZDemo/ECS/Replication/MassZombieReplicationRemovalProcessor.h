#pragma once

#include "MassObserverProcessor.h"

#include "MassZombieReplicationRemovalProcessor.generated.h"

/**
 * Ensures replicated zombie agents are removed from all client bubbles
 * when their server entities are destroyed.
 */
UCLASS()
class HZDEMO_API UMassZombieReplicationRemovalProcessor : public UMassObserverProcessor
{
	GENERATED_BODY()

public:
	UMassZombieReplicationRemovalProcessor();

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;
};

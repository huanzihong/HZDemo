#pragma once
#include "MassObserverProcessor.h"
#include "EnemyHashGridProcessor.generated.h"

//当有敌人生成时 加入Grid
UCLASS()
class UEnemyInitializer : public UMassObserverProcessor
{
	GENERATED_BODY()

public:
	UEnemyInitializer();
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery EntityQuery;
};

//删除
UCLASS()
class UEnemyDestructor : public UMassObserverProcessor
{
	GENERATED_BODY()

public:
	UEnemyDestructor();
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery EntityQuery;
};

//更新
UCLASS()
class HZDEMO_API UpdateEnemyHashGridProcessor : public UMassProcessor
{
	GENERATED_BODY()
public:
	UpdateEnemyHashGridProcessor();
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery UpdateHashGridQuery;
};

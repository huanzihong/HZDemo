#pragma once
#include "MassObserverProcessor.h"
#include "WeaponHashGridProcessor.generated.h"

//当有武器生成时 加入Grid
UCLASS()
class UWeaponInitializer : public UMassObserverProcessor
{
	GENERATED_BODY()

public:
	UWeaponInitializer();
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery EntityQuery;
};

//删除
UCLASS()
class UWeaponDestructor : public UMassObserverProcessor
{
	GENERATED_BODY()

public:
	UWeaponDestructor();
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery EntityQuery;
};



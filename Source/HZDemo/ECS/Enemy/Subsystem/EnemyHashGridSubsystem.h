#pragma once
#include "HierarchicalHashGrid2D.h"
#include "MassEntityHandle.h"
#include "EnemyHashGridSubsystem.generated.h"


typedef THierarchicalHashGrid2D<2, 4, FMassEntityHandle> FEnemyHashGrid;

UCLASS()
class HZDEMO_API UEnemyHashGridSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	const FEnemyHashGrid& GetHashGrid() const;
	FEnemyHashGrid& GetHashGrid_Mutable();
	
	// 射线查询最近的敌人
	bool RaycastEnemy(const FVector& RayStart, const FVector& RayDirection, float MaxDistance, FVector& OutHitLocation, FMassEntityHandle& OutHitEntity) const;
	
	FEnemyHashGrid EnemyHashGrid;
};
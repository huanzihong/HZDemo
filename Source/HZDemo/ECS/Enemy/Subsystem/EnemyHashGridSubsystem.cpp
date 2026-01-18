#include "EnemyHashGridSubsystem.h"

const FEnemyHashGrid& UEnemyHashGridSubsystem::GetHashGrid() const
{
	return EnemyHashGrid;
}

FEnemyHashGrid& UEnemyHashGridSubsystem::GetHashGrid_Mutable()
{
	return EnemyHashGrid;
}

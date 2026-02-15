#include "HashGridSubsystem.h"
#include "MassEntitySubsystem.h"
#include "MassExecutionContext.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"
#include "MassCommonFragments.h"

const FEnemyHashGrid& UHashGridSubsystem::GetHashGrid() const
{
	return EnemyHashGrid;
}

FEnemyHashGrid& UHashGridSubsystem::GetHashGrid_Mutable()
{
	return EnemyHashGrid;
}

bool UHashGridSubsystem::RaycastEnemy(const FVector& RayStart, const FVector& RayDirection, float MaxDistance, FVector& OutHitLocation, FMassEntityHandle& OutHitEntity) const
{
	UMassEntitySubsystem* EntitySubsystem = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
	if (!EntitySubsystem) return false;

	FVector RayEnd = RayStart + RayDirection * MaxDistance;
	
	float ClosestDistance = MaxDistance;
	bool bFoundHit = false;
	
	// 简单实现：查询射线起点和终点附近的敌人
	FBox SearchBox(ForceInit);
	SearchBox += RayStart;
	SearchBox += RayEnd;
	SearchBox = SearchBox.ExpandBy(500.f); // 扩展搜索范围
	
	// 使用HashGrid查询该区域的所有敌人
	TArray<FMassEntityHandle> NearbyEntities;
	EnemyHashGrid.Query(SearchBox, NearbyEntities);
	
	
	/*根据向量垂直则点积为零的性质，我们可以建立方程组。两点之间的向量应同时与射线方向 RayDir和胶囊体轴线方向 CapsuleAxis垂直。这便得到了一个关于 t和 s的线性方程组：
	[RayDir · ( (RayStart + t*RayDir) - (CapsuleBottom + s*CapsuleAxis) )] = 0
	[CapsuleAxis · ( (RayStart + t*RayDir) - (CapsuleBottom + s*CapsuleAxis) )] = 0
	令向量 StartToBottom = RayStart - CapsuleBottom，代入并展开点积运算（点积满足分配律），得到：
	RayDir · RayDir * t - RayDir · CapsuleAxis * s = -RayDir · StartToBottom
	CapsuleAxis · RayDir * t - CapsuleAxis · CapsuleAxis * s = -CapsuleAxis · StartToBottom
	这个方程组可以写成矩阵形式 A * x = b：*/
	
	// 对每个敌人做胶囊体射线相交测试
	FMassEntityManager& EntityManager = EntitySubsystem->GetMutableEntityManager();
	for (const FMassEntityHandle& Entity : NearbyEntities)
	{
		if (!EntityManager.IsEntityValid(Entity)) continue;

		const FTransformFragment* TransformFragment = EntityManager.GetFragmentDataPtr<FTransformFragment>(Entity);
		const FEnemyFragment* EnemyFragment = EntityManager.GetFragmentDataPtr<FEnemyFragment>(Entity);

		if (!TransformFragment || !EnemyFragment) continue;

		// 胶囊体参数
		FVector EnemyLocation = TransformFragment->GetTransform().GetLocation();
		float CapsuleRadius = EnemyFragment->CapsuleRadius;
		float CapsuleHalfHeight = EnemyFragment->CapsuleHalfHeight;

		// 计算胶囊体的顶部和底部中心点
		FVector CapsuleTop = EnemyLocation + FVector(0, 0, CapsuleHalfHeight);
		FVector CapsuleBottom = EnemyLocation - FVector(0, 0, CapsuleHalfHeight);

		// 绘制胶囊体调试信息
		//DrawDebugCapsule(GetWorld(), EnemyLocation, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, FColor::Cyan, false, 50.f, 0, 2.f);

		// 射线与胶囊体相交测试
		// 将问题简化为：射线与线段的最短距离是否小于半径
		FVector RayDir = (RayEnd - RayStart).GetSafeNormal();
		float RayLength = (RayEnd - RayStart).Size();

		// 计算射线上最接近胶囊体轴线的点
		FVector CapsuleAxis = CapsuleTop - CapsuleBottom;
		FVector StartToBottom = RayStart - CapsuleBottom;

		float a = FVector::DotProduct(RayDir, RayDir);
		float b = FVector::DotProduct(RayDir, CapsuleAxis);
		float c = FVector::DotProduct(CapsuleAxis, CapsuleAxis);
		float d = FVector::DotProduct(RayDir, StartToBottom);
		float e = FVector::DotProduct(CapsuleAxis, StartToBottom);

		float denom = a * c - b * b;
		float t = 0.f; // 射线参数
		float s = 0.f; // 胶囊体轴线参数

		if (denom != 0.f)
		{
			t = (b * e - c * d) / denom;
			s = (a * e - b * d) / denom;

			// 限制参数范围
			t = FMath::Clamp(t, 0.f, RayLength);
			s = FMath::Clamp(s, 0.f, 1.f);

			// 计算两条线段上的最近点
			FVector RayPoint = RayStart + RayDir * t;
			FVector CapsulePoint = CapsuleBottom + CapsuleAxis * s;

			float DistanceSquared = FVector::DistSquared(RayPoint, CapsulePoint);

			if (DistanceSquared <= (CapsuleRadius * CapsuleRadius))
			{
				// 射线击中胶囊体
				float Distance = t;
				if (Distance < ClosestDistance)
				{
					ClosestDistance = Distance;
					OutHitLocation = RayPoint;
					OutHitEntity = Entity;
					bFoundHit = true;

					// 绘制击中的胶囊体（高亮显示）
					//DrawDebugCapsule(GetWorld(), EnemyLocation, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, FColor::Orange, false, 1.f, 0, 3.f);
				}
			}
		}
	}
	
	return bFoundHit;
}

const FEnemyHashGrid& UHashGridSubsystem::GetWeaponHashGrid() const
{
	return WeaponHashGrid;
}

FEnemyHashGrid& UHashGridSubsystem::GetWeaponHashGrid_Mutable()
{
	return WeaponHashGrid;
}

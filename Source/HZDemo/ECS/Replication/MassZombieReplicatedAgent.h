#pragma once
#include "FMassReplicationNavPathHandler.h"
#include "MassClientBubbleHandler.h"
#include "MassReplicationTransformHandlers.h"
#include "ECS/Enemy/Traits/EnemyFragment.h"

#include "MassZombieReplicatedAgent.generated.h"
USTRUCT()
struct HZDEMO_API FReplicatedEnemyStateData
{
	GENERATED_BODY()

	void SetFromEnemyFragment(const FEnemyFragment& EnemyFragment)
	{
		EnemyState = EnemyFragment.EnemyState;
	}

	bool UpdateFromEnemyFragment(const FEnemyFragment& EnemyFragment)
	{
		if (EnemyState != EnemyFragment.EnemyState)
		{
			EnemyState = EnemyFragment.EnemyState;
			return true;
		}
		return false;
	}

	bool UpdateFromEnemyState(const EEnemyState InEnemyState)
	{
		if (EnemyState != InEnemyState)
		{
			EnemyState = InEnemyState;
			return true;
		}
		return false;
	}

	EEnemyState GetEnemyState() const { return EnemyState; }

private:
	UPROPERTY(Transient)
	EEnemyState EnemyState = EEnemyState::None;
};

USTRUCT()
struct HZDEMO_API FReplicatedZombieAgent : public FReplicatedAgentBase
{
	GENERATED_BODY()
	
	const FReplicatedAgentPositionYawData& GetReplicatedPositionYawData() const { return PositionYaw; }

	/** This function is required to be provided in FReplicatedAgentBase derived classes that use FReplicatedAgentPositionYawData */
	FReplicatedAgentPositionYawData& GetReplicatedPositionYawDataMutable() { return PositionYaw; }

	const FReplicatedAgentNavPathData& GetReplicatedNavPathData() const { return NavPath; }
	
	FReplicatedAgentNavPathData& GetReplicatedNavPathDataMutable() { return NavPath; }

	const FReplicatedEnemyStateData& GetReplicatedEnemyStateData() const { return EnemyStateData; }

	FReplicatedEnemyStateData& GetReplicatedEnemyStateDataMutable() { return EnemyStateData; }

private:
	UPROPERTY(Transient)
	FReplicatedAgentPositionYawData PositionYaw;
	UPROPERTY(Transient)
	FReplicatedAgentNavPathData NavPath;
	UPROPERTY(Transient)
	FReplicatedEnemyStateData EnemyStateData;
};

USTRUCT()
struct HZDEMO_API FZombieFastArrayItem : public FMassFastArrayItemBase
{
	GENERATED_BODY()

	FZombieFastArrayItem() = default;
	FZombieFastArrayItem(const FReplicatedZombieAgent& InAgent, const FMassReplicatedAgentHandle InHandle)
		: FMassFastArrayItemBase(InHandle)
		, Agent(InAgent)
	{}

	/** This typedef is required to be provided in FMassFastArrayItemBase derived classes (with the associated FReplicatedAgentBase derived class) */
	typedef FReplicatedZombieAgent FReplicatedAgentType;

	UPROPERTY()
	FReplicatedZombieAgent Agent;
};

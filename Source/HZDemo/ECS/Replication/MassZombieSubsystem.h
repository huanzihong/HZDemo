#pragma once
#include "MassSubsystemBase.h"

#include "MassZombieSubsystem.generated.h"

UCLASS()
class HZDEMO_API UMassZombieSubsystem : public UMassSubsystemBase
{
	GENERATED_BODY()
public:
	virtual void PostInitialize() override;
};


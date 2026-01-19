#pragma once
#include "GameFramework/PlayerState.h"
#include "HZPlayerState.generated.h"
class UHZAttributeSet;
class UHZAbilitySystemComponent;

UCLASS()
class HZDEMO_API AHZPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AHZPlayerState();
	
	virtual void PostInitializeComponents() override;

	UHZAbilitySystemComponent* GetAbilitySystemComponent();
	
	UPROPERTY()
	TObjectPtr<UHZAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UHZAttributeSet> AttributeSetBase;
	
};

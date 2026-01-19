#include "HZPlayerState.h"

#include "GAS/HZAbilitySystemComponent.h"
#include "GAS/HZAttributeSet.h"

AHZPlayerState::AHZPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UHZAbilitySystemComponent>("AbilitySystemComponent");
	
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);

	AttributeSetBase = CreateDefaultSubobject<UHZAttributeSet>(TEXT("AttributeSetBase"));
}

void AHZPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	check(AbilitySystemComponent);
	//AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
}

UHZAbilitySystemComponent* AHZPlayerState::GetAbilitySystemComponent()
{
	return AbilitySystemComponent;
}

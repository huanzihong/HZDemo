#pragma once
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "HZAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class UHZAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Attribute", DisplayName = "生命上限")
	FGameplayAttributeData MaxHp;
	ATTRIBUTE_ACCESSORS(UHZAttributeSet, MaxHp)

	UPROPERTY(BlueprintReadOnly, Category = "Attribute", DisplayName = "生命值")
	FGameplayAttributeData Hp;
	ATTRIBUTE_ACCESSORS(UHZAttributeSet, Hp)
	
};

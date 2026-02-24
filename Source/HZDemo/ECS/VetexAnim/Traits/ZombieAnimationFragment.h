#pragma once
#include "AnimToTextureDataAsset.h"
#include "MassEntityElementTypes.h"
#include "MassEntityConcepts.h"
#include "ZombieAnimationFragment.generated.h"

USTRUCT()
struct HZDEMO_API FZombieAnimationFragment : public FMassFragment
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimToTextureDataAsset> AnimToTextureData;
	
	float GlobalStartTime = 0.0f;
	float PlayRate = 1.0f;
	int32 AnimationStateIndex = 0;
	//bool bSwappedThisFrame = false;
};

template<>
struct TMassFragmentTraits<FZombieAnimationFragment> final
{
	enum
	{
		AuthorAcceptsItsNotTriviallyCopyable = true
	};
};
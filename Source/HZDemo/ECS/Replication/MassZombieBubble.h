#pragma once
#include "MassClientBubbleInfoBase.h"
#include "MassClientBubbleSerializerBase.h"
#include "MassZombieReplicatedAgent.h"

#include "MassZombieBubble.generated.h"

class HZDEMO_API FMassZombieClientBubbleHandler : public TClientBubbleHandlerBase<FZombieFastArrayItem>
{
public:
	typedef TClientBubbleHandlerBase<FZombieFastArrayItem> Super;
	typedef TMassClientBubbleNavPathHandler<FZombieFastArrayItem> FMassClientBubbleNavPathHandler;
	typedef TMassClientBubbleTransformHandler<FZombieFastArrayItem> FMassClientBubbleTransformHandler;

	FMassZombieClientBubbleHandler()
		: NavPathHandler(*this)
		, TransformHandler(*this)
	{}

#if UE_REPLICATION_COMPILE_SERVER_CODE
	
	const FMassClientBubbleTransformHandler& GetTransformHandler() const { return TransformHandler; }
	FMassClientBubbleTransformHandler& GetTransformHandlerMutable() { return TransformHandler; }
	const FMassClientBubbleNavPathHandler& GetNavPathHandler() const { return NavPathHandler; }
	FMassClientBubbleNavPathHandler& GetNavPathHandlerMutable() { return NavPathHandler; }
#endif // UE_REPLICATION_COMPILE_SERVER_CODE


protected:
#if UE_REPLICATION_COMPILE_CLIENT_CODE
	virtual void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize) override;
	virtual void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize) override;

	//void PostReplicatedChangeEntity(const FMassEntityView& EntityView, const FReplicatedZombieAgent& Item) const;
#endif //UE_REPLICATION_COMPILE_CLIENT_CODE
	
	FMassClientBubbleNavPathHandler NavPathHandler;
	FMassClientBubbleTransformHandler TransformHandler;
};

USTRUCT()
struct HZDEMO_API FMassZombieClientBubbleSerializer : public FMassClientBubbleSerializerBase
{
	GENERATED_BODY()

	FMassZombieClientBubbleSerializer()
	{
		Bubble.Initialize(Zombie, *this);
	};
		
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FZombieFastArrayItem, FMassZombieClientBubbleSerializer>(Zombie, DeltaParams, *this);
	}

public:
	FMassZombieClientBubbleHandler Bubble;

protected:
	/** Fast Array of Agents for efficient replication. Maintained as a freelist on the server, to keep index consistency as indexes are used as Handles into the Array 
	 *  Note array order is not guaranteed between server and client so handles will not be consistent between them, FMassNetworkID will be.
	 */
	UPROPERTY(Transient)
	TArray<FZombieFastArrayItem> Zombie;
};

template<>
struct TStructOpsTypeTraits<FMassZombieClientBubbleSerializer> : public TStructOpsTypeTraitsBase2<FMassZombieClientBubbleSerializer>
{
	enum
	{
		WithNetDeltaSerializer = true,
		WithCopy = false,
	};
};

UCLASS()
class HZDEMO_API AMassZombieClientBubbleInfo : public AMassClientBubbleInfoBase
{
	GENERATED_BODY()

public:
	AMassZombieClientBubbleInfo(const FObjectInitializer& ObjectInitializer);

	FMassZombieClientBubbleSerializer& GetZombieSerializer() { return ZombieSerializer; }

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Replicated, Transient)
	FMassZombieClientBubbleSerializer ZombieSerializer;
};

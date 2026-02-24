#include "MassZombieSubsystem.h"
#include "MassReplicationSubsystem.h"
#include "MassZombieBubble.h"

void UMassZombieSubsystem::PostInitialize()
{
	UMassSubsystemBase::PostInitialize();
	UMassReplicationSubsystem* ReplicationSubsystem = UWorld::GetSubsystem<UMassReplicationSubsystem>(GetWorld());

	check(ReplicationSubsystem);
	ReplicationSubsystem->RegisterBubbleInfoClass(AMassZombieClientBubbleInfo::StaticClass());
}



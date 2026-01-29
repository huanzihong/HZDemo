#include "Weapon.h"

#include "Components/SphereComponent.h"
#include "HZDemoCharacter.h"
#include "Player/CombatComponent.h"

AWeapon::AWeapon()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AHZDemoCharacter* Character = Cast<AHZDemoCharacter>(OtherActor))
	{
		if (UCombatComponent* Combat = Character->GetCombatComponent())
		{
			Combat->SetOverlappingWeapon(this);
		}
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AHZDemoCharacter* Character = Cast<AHZDemoCharacter>(OtherActor))
	{
		if (UCombatComponent* Combat = Character->GetCombatComponent())
		{
			if (Combat->GetOverlappingWeapon() == this)
			{
				Combat->SetOverlappingWeapon(nullptr);
			}
		}
	}
}

void AWeapon::SetWeaponState(bool bEquipped)
{
	if (bEquipped)
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Keep mesh collision off for now to avoid physics issues
	}
}

USkeletalMeshComponent* AWeapon::GetWeaponMesh()
{
	return WeaponMesh;
}

UMassEntityConfigAsset* AWeapon::GetBulletConfig()
{
	return BulletConfig;
}

// Copyright Epic Games, Inc. All Rights Reserved.

#include "HZDemoCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "HZDemo.h"
#include "ECS/Bullet/BulletSubsystem.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GAS/HZAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CombatComponent.h"
#include "Player/HZPlayerState.h"
#include "Weapon/Weapon.h"
#include "ECS/Enemy/Subsystem/EnemyHashGridSubsystem.h"

AHZDemoCharacter::AHZDemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));

	CurrentFOV = DefaultFOV;
}

void AHZDemoCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CombatComponent && FollowCamera)
	{
		float TargetFOV = CombatComponent->GetAimming() ? AimedFOV : DefaultFOV;
		CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaSeconds, FOVInterpSpeed);
		FollowCamera->SetFieldOfView(CurrentFOV);
	}
}

void AHZDemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHZDemoCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AHZDemoCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHZDemoCharacter::Look);

		//Shooting
		EnhancedInputComponent->BindAction(ShootAction,ETriggerEvent::Started,this,&AHZDemoCharacter::Shoot);

		EnhancedInputComponent->BindAction(AimAction,ETriggerEvent::Triggered,this,&AHZDemoCharacter::Aimming);
		EnhancedInputComponent->BindAction(AimAction,ETriggerEvent::Completed,this,&AHZDemoCharacter::Aimming);
		
		EnhancedInputComponent->BindAction(EquipAction,ETriggerEvent::Started,this,&AHZDemoCharacter::Equip);
	}
	else
	{
		UE_LOG(LogHZDemo, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AHZDemoCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AHZDemoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AHZDemoCharacter::Shoot(const FInputActionValue& Value)
{
	if(auto BulletSys = GetWorld()->GetSubsystem<UBulletSubsystem>())
	{
		
		if(auto EquippedWeapon = CombatComponent->GetEquippedWeapon())
		{
			if(auto BulletConfig = EquippedWeapon->GetBulletConfig())
			{
				//获取枪口位置
				const USkeletalMeshSocket* MuzzleFlashSocket = EquippedWeapon->GetWeaponMesh()->GetSocketByName("MuzzleFlash");
				if (MuzzleFlashSocket == nullptr) return;

				const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(EquippedWeapon->GetWeaponMesh());
				FVector MuzzleLocation = SocketTransform.GetLocation();
				
				// 获取屏幕中心准星位置
				FVector2D ViewportSize;
				if (GEngine && GEngine->GameViewport)
				{
					GEngine->GameViewport->GetViewportSize(ViewportSize);
				}
				FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
				FVector CrosshairWorldPosition;
				FVector CrosshairWorldDirection;
				bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
					UGameplayStatics::GetPlayerController(this, 0),
					CrosshairLocation,
					CrosshairWorldPosition,
					CrosshairWorldDirection
				);
				
				if (bScreenToWorld)
				{
					FVector TargetLocation;
					bool bHitEnemy = false;
					
					// 先尝试用HashGrid查询Mass敌人
					if (UEnemyHashGridSubsystem* EnemyGridSys = GetWorld()->GetSubsystem<UEnemyHashGridSubsystem>())
					{
						FVector HitLocation;
						FMassEntityHandle HitEntity;
						if (EnemyGridSys->RaycastEnemy(CrosshairWorldPosition, CrosshairWorldDirection, 80000.f, HitLocation, HitEntity))
						{
							TargetLocation = HitLocation;
							bHitEnemy = true;
							DrawDebugSphere(GetWorld(), TargetLocation, 20.f, 12, FColor::Green, false, 2.f);
						}
					}
					
					// 如果没击中Mass敌人，再检测普通碰撞体（如环境）
					if (!bHitEnemy)
					{
						FVector TraceStart = CrosshairWorldPosition;
						FVector TraceEnd = CrosshairWorldPosition + CrosshairWorldDirection * 80000.f;
						
						FHitResult HitResult;
						FCollisionQueryParams QueryParams;
						QueryParams.AddIgnoredActor(this);
						QueryParams.AddIgnoredActor(EquippedWeapon);
						
						bool bHit = GetWorld()->LineTraceSingleByChannel(
							HitResult,
							TraceStart,
							TraceEnd,
							ECC_Visibility,
							QueryParams
						);
						
						if (bHit)
						{
							TargetLocation = HitResult.ImpactPoint;
							DrawDebugSphere(GetWorld(), TargetLocation, 10.f, 12, FColor::Blue, false, 2.f);
						}
						else
						{
							TargetLocation = TraceEnd;
						}
					}
					
					// 计算从枪口到目标的方向
					FVector ShootDirection = (TargetLocation - MuzzleLocation).GetSafeNormal();
					
					DrawDebugSphere(GetWorld(), MuzzleLocation, 10.f, 12, FColor::Red, false, 2.f);
					DrawDebugLine(GetWorld(), MuzzleLocation, TargetLocation, FColor::Yellow, false, 2.f, 0, 2.f);
					
					BulletSys->SpawnBullet(BulletConfig, MuzzleLocation, ShootDirection);
				}
				
			}
		}
		
	}
}

void AHZDemoCharacter::Aimming(const FInputActionValue& Value)
{
	bool bAim = Value.Get<bool>();
	
	CombatComponent->SetAimming(bAim);
	
}

void AHZDemoCharacter::Equip(const FInputActionValue& Value)
{
	if (CombatComponent)
	{
		CombatComponent->EquipWeapon(CombatComponent->GetOverlappingWeapon());
	}
}

void AHZDemoCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AHZDemoCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AHZDemoCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AHZDemoCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AHZDemoCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AHZPlayerState* PS = GetPlayerState<AHZPlayerState>();
	if (PS)
	{
		
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		//auto AbilitySystemComponent = Cast<UHZAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
	}
}

UHZAbilitySystemComponent* AHZDemoCharacter::GetAbilitySystemComponent()
{
	AHZPlayerState* PS = GetPlayerState<AHZPlayerState>();
	if (PS)
	{
		return PS->GetAbilitySystemComponent();
	}else
	{
		return nullptr;
	}
}

UCombatComponent* AHZDemoCharacter::GetCombatComponent()
{
	return CombatComponent;
}

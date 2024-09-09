// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASShooterCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Public/GAS/ShootAbilitySystemComponent.h"
#include "Public/Player/ShootPlayerState.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AGASShooterCharacter

AGASShooterCharacter::AGASShooterCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterFPMesh"));
	FPMesh->SetOnlyOwnerSee(true);
	FPMesh->SetupAttachment(FirstPersonCameraComponent);
	FPMesh->bCastDynamicShadow = false;
	FPMesh->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	FPMesh->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	// Third person mesh
	FullBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FullBody"));
	FullBody->SetupAttachment(GetCapsuleComponent());

}

void AGASShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AGASShooterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeActorInfo();
}

void AGASShooterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeActorInfo();
}

//////////////////////////////////////////////////////////////////////////// Input

void AGASShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGASShooterCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGASShooterCharacter::Look);
	}
	else
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	
}

void AGASShooterCharacter::InitializeActorInfo()
{
	AShootPlayerState* PS = GetPlayerState<AShootPlayerState>();
	
	if (PS)
	{
		AbilitySystemComponent = Cast<UShootAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
		AbilitySystemComponent = PS->GetAbilitySystemComponent();
	}
}


void AGASShooterCharacter::GrabWeapon(EWeaponType WeaponType)
{
	switch (WeaponType)
	{
		case EWeaponType::Pistol :
			bHasPistol = true;
		break;
		
		case EWeaponType::Principal :
			bHasPrincipal = true;
		break;
		default: ;
	}
}

UAbilitySystemComponent* AGASShooterCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void AGASShooterCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AGASShooterCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
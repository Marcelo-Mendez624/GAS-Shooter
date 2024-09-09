// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "GASShooterCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);

	ShootDistance = 5000;
}


void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
		return;
	
	if (FireSound != nullptr)
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	
	if (FireAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = Character->GetFPMesh()->GetAnimInstance();
		 
		if (AnimInstance != nullptr)
			AnimInstance->Montage_Play(FireAnimation, 1.f);
	}
	
	FVector StartLocation = Character->GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector EndLocation = StartLocation + (Character->GetFirstPersonCameraComponent()->GetForwardVector() * ShootDistance);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	
	TArray<FHitResult> HitResults;
	
	bool bHit = GetWorld()->LineTraceMultiByChannel(HitResults, StartLocation, EndLocation,ECC_Visibility, QueryParams);
	
	if (bHit)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			if (HitResult.GetActor() != nullptr)
				UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
		}
	}

	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, 2);
	
}

void UTP_WeaponComponent::InspectWeapon()
{
}

bool UTP_WeaponComponent::AttachWeapon(AGASShooterCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr)
		return false;

	if(WeaponType == EWeaponType::Pistol && Character->GetHasPistol())
		return false;
	
	if(WeaponType == EWeaponType::Principal && Character->GetHasPrincipal())
		return false;
	
	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	if(Character->IsLocallyControlled())
		AttachToComponent(Character->GetFPMesh(), AttachmentRules, FName(TEXT("GripPoint")));
	else
		AttachToComponent(Character->GetFullBody(), AttachmentRules, FName(TEXT("GripPoint")));
	// add the weapon as an instance component to the character
	Character->AddInstanceComponent(this);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);

			// Inspect
			EnhancedInputComponent->BindAction(InspectAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::InspectWeapon);
		}
	}
	
	
	return true;
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
		return;
	
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->RemoveMappingContext(FireMappingContext);

}
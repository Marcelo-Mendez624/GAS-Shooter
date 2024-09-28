// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ShootDataTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_WeaponActor.generated.h"

class UGameplayAbility;
class AGASShooterCharacter;
class UInputAction;
class UInputMappingContext;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GASSHOOTER_API AUtp_WeaponComponent : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AUtp_WeaponComponent();
	
	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool AttachWeapon(AGASShooterCharacter* TargetCharacter);
	
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void InspectWeapon();

	void AddDefaultAbilities();
	

protected:
	/** Weapon Variables **/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	EWeaponType WeaponType;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float ShootDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FVector MuzzleOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	USoundBase* FireSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	UAnimMontage* FireAnimation;
	
	/** Input variables **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* FireMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* FireAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* InspectAction;

	/** Ability System **/
	UPROPERTY(BlueprintReadOnly)
	UAbilitySystemComponent* AbilitySystemComponent;
	
	
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;



private:
	UPROPERTY(VisibleAnywhere)
	AGASShooterCharacter* Character;

	/** Get Section **/
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "GASShooter/Public/Player/ShootPlayerState.h"

#include "AbilitySystemComponent.h"

AShootPlayerState::AShootPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
}

UAbilitySystemComponent* AShootPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
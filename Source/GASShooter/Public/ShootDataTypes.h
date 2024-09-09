#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None UMETA(DisplayName = "Default"),
	Pistol   UMETA(DisplayName = "Pistol"),
	Principal      UMETA(DisplayName = "Principal"),
};

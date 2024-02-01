//Reminder on how to include this file in other classes:
//#include "Characters/CharacterTypes.h"

#pragma once
UENUM(BlueprintType)
enum class ECharacterState : uint8 {
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8 {
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	//Used for both equipping and unequiping.
	EAS_EquippingWeapon UMETA(DisplayName = "EquippingWeapon")
};


UENUM(BlueprintType)
enum class EDeathPose : uint8 {
	EDP_Alive = 0 UMETA(DisplayName = "Alive") ,
	EDP_Death1 = 1  UMETA(DisplayName = "Death1"),
	EDP_Death2 = 2 UMETA(DisplayName = "Death2"),
	EDP_Death3 = 3 UMETA(DisplayName = "Death3"),
	EDP_Death4 = 4 UMETA(DisplayName = "Death4"),
	EDP_Death5 = 5 UMETA(DisplayName = "Death5"),
	EDP_Death6 = 6 UMETA(DisplayName = "Death6"),
};
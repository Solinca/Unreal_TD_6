#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Global/MyGameInstance.h"
#include "MonsterDataAsset.generated.h"

UCLASS()
class TD_6_API UMonsterDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EMonsterType> MonsterType = EMonsterType::BUTCHER;

	UPROPERTY(EditDefaultsOnly)
	FString MonsterName = "BUTCHER";

	UPROPERTY(EditDefaultsOnly)
	float MonsterAttackCooldown = 1.f;

	UPROPERTY(EditDefaultsOnly)
	float MonsterSpecialCooldown = 2.f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "MonsterType == EMonsterType::BUTCHER"))
	float HookSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "MonsterType == EMonsterType::BUTCHER"))
	float HookReelingTime = 2.f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "MonsterType == EMonsterType::HUNTER"))
	float TrapSnareTime = 3.f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "MonsterType == EMonsterType::HUNTER"))
	float MaxNumberOfTraps = 5.f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "MonsterType == EMonsterType::GHOST"))
	float GhostInvisibilityDuration = 3.f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "MonsterType == EMonsterType::SLIME"))
	float SlimeSprintSpeed = 1500.f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "MonsterType == EMonsterType::SLIME"))
	float SlimeTransformationDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "MonsterType == EMonsterType::PREDATOR"))
	float PredatorHuntDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Scream")
	USoundBase* ScreamSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Scream")
	USoundBase* MonsterRevealCrySound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Scream")
	USoundAttenuation* ScreamAttenuation = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Scream",
		meta = (ClampMin = "1.0", ClampMax = "60.0"))
	float ScreamCooldown = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Scream",
		meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MonsterCryChance = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	USoundBase* AbilityTriggerSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	USoundBase* AbilityOnGoingSound = nullptr;
};

#pragma once

#include "CoreMinimal.h"
#include "Player/Capacity/BaseAbilityComponent.h"
#include "HookAbilityComponent.generated.h"

class AHookProjectile;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TD_6_API UHookAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Hook")
	TSubclassOf<AHookProjectile> HookProjectileClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Hook")
	FVector SpawnOffset = FVector(0.f, 0.f, 50.f);

	TWeakObjectPtr<AHookProjectile> ActiveProjectile = nullptr;

public:
	UHookAbilityComponent();

	virtual void ActivateAbility() override;
	virtual void DeactivateAbility() override;

	void OnHookFinished();
};
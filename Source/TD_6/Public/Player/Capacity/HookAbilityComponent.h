#pragma once

#include "CoreMinimal.h"
#include "Player/Capacity/BaseAbilityComponent.h"
#include "HookAbilityComponent.generated.h"

class AHookProjectile;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TD_6_API UHookAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

private:
	TWeakObjectPtr<class AMyCharacter> MyChara = nullptr;

	TObjectPtr<AHookProjectile> ActiveProjectile = nullptr;

protected:
	UHookAbilityComponent();

	UPROPERTY(EditDefaultsOnly, Category = "Hook")
	TSubclassOf<AHookProjectile> HookProjectileClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Hook")
	FVector SpawnOffset = FVector(0.f, 0.f, 50.f);

public:
	virtual void ActivateAbility() override;

	virtual void DeactivateAbility() override {};
};

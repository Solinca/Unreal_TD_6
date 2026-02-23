#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class TD_6_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	TObjectPtr<AActor> InteractingActor = nullptr;

	UPROPERTY(ReplicatedUsing = SetFlashlightVisibility)
	bool IsFlashlightOn = false;

	UFUNCTION()
	void SetFlashlightVisibility();

protected:
	AMyCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UCameraComponent> Camera = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USpringArmComponent> SpringArm = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USpotLightComponent> Flashlight = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float InteractRange = 200.f;

public:
	UFUNCTION(Server, Reliable)
	void ToggleFlashlight();

	UFUNCTION(Server, Reliable)
	void InteractWithSurroundingActor();

	UFUNCTION(Server, Reliable)
	void StopInteractingWithActor();
};

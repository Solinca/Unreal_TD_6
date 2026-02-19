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

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UCameraComponent> Camera = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USpringArmComponent> SpringArm = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USpotLightComponent> Flashlight = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float InteractRange = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UPostProcessComponent> PostProcess = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	FPostProcessSettings DefaultPostProcess;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	FPostProcessSettings NightVisionPostProcess;

public:
	UFUNCTION(Server, Reliable)
	void ToggleFlashlight();

	UFUNCTION(Server, Reliable)
	void InteractWithSurroundingActor();

	UFUNCTION(Server, Reliable)
	void StopInteractingWithActor();

	UFUNCTION(BlueprintCallable)
	void ChangePostProcess(const bool bIsDefault = true) const;
};
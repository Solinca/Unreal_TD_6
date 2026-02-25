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

	FTimerHandle AttackHandle;

	bool IsAttacking = false;

	UPROPERTY(ReplicatedUsing = SetFlashlightVisibility)
	bool IsFlashlightOn = false;

	UFUNCTION()
	void SetFlashlightVisibility();

	UPROPERTY(ReplicatedUsing = OnPlayerDeath)
	bool IsDead = false;

	UFUNCTION()
	void OnPlayerDeath();

	void ResetAttacking();

protected:
	AMyCharacter();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UCameraComponent> Camera = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USpringArmComponent> SpringArm = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USpotLightComponent> Flashlight = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UPostProcessComponent> PostProcess = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	FPostProcessSettings DefaultPostProcess;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	FPostProcessSettings NightVisionPostProcess;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float InteractRange = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float AttackAnimationTime = 1.f;

public:
	UFUNCTION(Server, Reliable)
	void ToggleFlashlight();

	UFUNCTION(Server, Reliable)
	void InteractWithSurroundingActor();

	UFUNCTION(Server, Reliable)
	void StopInteractingWithActor();

	UFUNCTION(BlueprintCallable)
	void ChangePostProcess(const bool bIsDefault = true) const;

	void SetIsAttacking();

	void KillPlayer();

	bool IsPlayerDead() { return IsDead; };

	UCameraComponent* GetCameraComponent() const { return Camera; }
};

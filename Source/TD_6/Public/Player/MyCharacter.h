#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/Interactable.h"
#include "MyCharacter.generated.h"

UCLASS()
class TD_6_API AMyCharacter : public ACharacter, public IInteractable
{
	GENERATED_BODY()

private:
	TObjectPtr<class AMyBaseLevelGameState> MyBLGS = nullptr;

	TObjectPtr<AActor> InteractingActor = nullptr;

	FTimerHandle AttackHandle;

	int PlayerInteractingWithCount = 0;

	bool IsAttacking = false;

	bool IsResurrectionComplete = false;

	bool HasToForceDisableProgressBar = false;

	void ResetAttacking();

	UPROPERTY(ReplicatedUsing = SetFlashlightVisibility)
	bool IsFlashlightOn = false;

	UFUNCTION()
	void SetFlashlightVisibility();

	void DisableFlashlight();

	UFUNCTION(NetMulticast, Reliable)
	void ForceDisableProgressBarOnAllClients();

	UPROPERTY(ReplicatedUsing = OnPlayerDeathStatusChanged)
	bool IsDead = false;

	UFUNCTION()
	void OnPlayerDeathStatusChanged();

	UPROPERTY(ReplicatedUsing = DisplayResurrectProgression)
	float ResurrectProgression = 0;

	UFUNCTION()
	void DisplayResurrectProgression();

	void StopCollidingWithCamera();

protected:
	AMyCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWidgetComponent> ProgressBar = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float InteractRange = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float AttackAnimationTime = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float ResurrectDuration = 5;

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

	void ResurrectPlayer();

	bool IsPlayerDead() { return IsDead; };

	UCameraComponent* GetCameraComponent() const { return Camera; }

	virtual bool InteractWith() override;

	virtual void StopInteractWith() override;

	void PreparePlayerForEndScreen();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsAttacking() const { return IsAttacking; }
};

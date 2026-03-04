#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "PlayerObjective.generated.h"

UCLASS()
class TD_6_API APlayerObjective : public AActor, public IInteractable
{
	GENERATED_BODY()

private:
	UPROPERTY(Transient)
	TObjectPtr<class UAudioComponent> OnGoingAudioComponent = nullptr;

	bool IsCompleted = false;

	int PlayerInteractingWithCount = 0;

	UPROPERTY(ReplicatedUsing = DisplayObjectiveProgression)
	float ObjectiveProgression = 0;

	UFUNCTION()
	void DisplayObjectiveProgression();

	UFUNCTION(NetMulticast, Reliable)
	void ToggleEffects(const bool bShouldActivate);

protected:
	APlayerObjective();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWidgetComponent> ProgressBar = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UNiagaraComponent> NiagaraComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<class UNiagaraSystem> Vfx = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	TObjectPtr<class USoundBase> SFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float ObjectiveGoal = 30;

public:
	virtual bool InteractWith() override;

	virtual void StopInteractWith() override;
};

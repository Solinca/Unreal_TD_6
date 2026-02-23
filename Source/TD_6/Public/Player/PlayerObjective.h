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
	bool IsCompleted = false;

	int PlayerInteractingWithCount = 0;

	UPROPERTY(ReplicatedUsing = DisplayObjectiveProgression)
	float ObjectiveProgression = 0;

	UFUNCTION()
	void DisplayObjectiveProgression();

protected:
	APlayerObjective();

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float ObjectiveGoal = 60;

public:
	virtual bool InteractWith() override;

	virtual void StopInteractWith() override;
};

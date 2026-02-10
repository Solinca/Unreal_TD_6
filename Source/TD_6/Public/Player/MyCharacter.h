#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class TD_6_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	AMyCharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UCameraComponent> Camera = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USpringArmComponent> SpringArm = nullptr;
};

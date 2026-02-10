#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputLibrary.h"
#include "MyPlayerController.generated.h"

USTRUCT(BlueprintType)
struct FInputData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> Action;

	UPROPERTY(EditAnywhere)
	ETriggerEvent Event;

	UPROPERTY(EditAnywhere, meta = (FunctionReference, PrototypeFunction = "/Script/TD_6.MyPlayerController.Prototype_InputAction"))
	FMemberReference ActionName;
};

UCLASS()
class TD_6_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	TObjectPtr<class AMyCharacter> MyChara = nullptr;

	TObjectPtr<class UPauseMenuWidget> PauseWidget = nullptr;

	FInputModeGameOnly GameOnly;

	FInputModeUIOnly UIOnly;

	float DefaultMaxSpeed;

	bool IsPauseMenuOpened = false;

	UFUNCTION()
	void OnContinueButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();

protected:
	AMyPlayerController();

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Mapping")
	TObjectPtr<UInputMappingContext> MappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Data")
	TArray<FInputData> InputDataList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Settings")
	TSubclassOf<UUserWidget> PauseWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Settings")
	float CameraSensitivity = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Settings")
	float PlayerSprintFactor = 2.f;

#if WITH_EDITOR
	UFUNCTION(BlueprintInternalUseOnly)
	void Prototype_InputAction(const FInputActionValue& Value) {};
#endif

	UFUNCTION(BlueprintCallable)
	void Move(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void Look(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void Jump(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void SprintStart(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void SprintEnd(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void ToggleMenu(const FInputActionValue& Value);
};

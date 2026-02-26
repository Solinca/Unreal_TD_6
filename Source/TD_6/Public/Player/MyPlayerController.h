#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputLibrary.h"
#include "Global/MyGameInstance.h"
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

	TObjectPtr<class UWaitingScreenWidget> WaitingScreenWidget = nullptr;

	TObjectPtr<class UMonsterDataAsset> MyMonsterData;

	FCustomPlayerData CustomPlayerData;

	FTimerHandle ResetAttackHandle;

	FTimerHandle ResetSpecialHandle;

	FInputModeGameOnly GameOnly;

	FInputModeUIOnly UIOnly;

	float DefaultMaxSpeed;

	bool IsPauseMenuOpened = false;

	bool CanAttack = true;

	bool CanTriggerSpecial = true;

	bool IsInteracting = false;

	void OnWaitingComplete();

	UFUNCTION(Server, Reliable)
	void RegisterReadyToGameState();

	UFUNCTION()
	void OnContinueButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();

	void SetupInput(TArray<FInputData> InputDataList);

	UFUNCTION(Server, Reliable)
	void AskToTriggerAttack();

	void ResetAttack();

	UFUNCTION(Server, Reliable)
	void AskToTriggerSpecial();

	void ResetSpecial();

	UFUNCTION(Server, Reliable)
	void SetIsSprintingOnServer(bool IsSprinting);

protected:
	AMyPlayerController();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Mapping")
	TObjectPtr<UInputMappingContext> MappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Data")
	TArray<FInputData> CommonInputDataList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Data")
	TArray<FInputData> PlayerInputDataList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Data")
	TArray<FInputData> MonsterInputDataList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Settings")
	float CameraSensitivity = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Settings")
	float PlayerSprintFactor = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> WaitingScreenWidgetClass = nullptr;

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

	UFUNCTION(BlueprintCallable)
	void CrouchStart(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void CrouchEnd(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void ToggleFlashlight(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void InteractStart(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void InteractStop(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void TriggerAttack(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void TriggerSpecial(const FInputActionValue& Value);

public:
	UFUNCTION(Server, Reliable)
	void SetupServer(FCustomPlayerData Data, class UMonsterDataAsset* MonsterData);

	UFUNCTION(Client, Reliable)
	void SetupClient(FCustomPlayerData Data, class UMonsterDataAsset* MonsterData, int WaitingTime);

	UFUNCTION(Client, Reliable)
	void ToggleWaitingScreenOff();

	UFUNCTION(Client, Reliable)
	void DisplayCountdown(int Countdown);

	UFUNCTION(Client, Reliable)
	void DisplayGlobalTimer(int Countdown);

	UFUNCTION(Client, Reliable)
	void DestroySessionOnClient();

	UFUNCTION(Server, Reliable)
	void KillPlayer();

	UFUNCTION(Server, Reliable)
	void ResurrectPlayer();

	UFUNCTION(Server, Reliable)
	void DisplayResultScreenServer(FVector TargetPosition);

	UFUNCTION(Client, Reliable)
	void DisplayResultScreenClient(ETeam WinningTeam);
};

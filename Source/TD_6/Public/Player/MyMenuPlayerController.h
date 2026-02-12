#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyMenuPlayerController.generated.h"

UCLASS()
class TD_6_API AMyMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	TObjectPtr<class UOnlineSessionSubsystem> OnlineSessionSubsystem = nullptr;

	TObjectPtr<class UMainMenuWidget> MainMenuWidget = nullptr;

	TObjectPtr<class ULobbySelectionWidget> LobbySelectionWidget = nullptr;

	FInputModeUIOnly UIOnly;

	int CurrentlySelectedLobbyID = -1;

	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
	void OnFindGameSessionComplete(const TArray<FCustomSessionInfo>& SessionResults, bool HasSuccess);

	UFUNCTION()
	void OnQuitButtonClicked();

	UFUNCTION()
	void OnCreateLobbyButtonClicked();

	UFUNCTION()
	void OnJoinLobbyButtonClicked();

	UFUNCTION()
	void OnRefreshLobbyButtonClicked();

	UFUNCTION()
	void OnLobbySelected(int LobbyID);

protected:
	AMyMenuPlayerController();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LobbySelectionWidgetClass = nullptr;
};

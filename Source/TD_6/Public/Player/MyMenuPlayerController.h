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

	TObjectPtr<class USessionCreationWidget> SessionCreationWidget = nullptr;

	TObjectPtr<class UJoinSessionWidget> JoinSessionWidget = nullptr;

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

	UFUNCTION()
	void OnSessionCreationConfirmed(const FString& SessionName, const FString& Username, int32 MaxPlayers, int32 MaxMonsters, bool IsLan);

	UFUNCTION()
	void OnSessionCreationCancelled();

	UFUNCTION()
	void OnJoinSessionConfirmed(const FString& Username);

	UFUNCTION()
	void OnJoinSessionCancelled();

	UFUNCTION()
	void OnJoinSessionFailed(FString ErrorMessage);

protected:
	AMyMenuPlayerController();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LobbySelectionWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> SessionCreationWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> JoinSessionWidgetClass = nullptr;
};

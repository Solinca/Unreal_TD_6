#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Global/MyGameInstance.h"
#include "MyLobbyPlayerController.generated.h"

UCLASS()
class TD_6_API AMyLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	TObjectPtr<class ULobbyManagementWidget> LobbyManagementWidget = nullptr;

	FInputModeUIOnly UIOnly;

	UFUNCTION()
	void OnStartButtonClicked();

	UFUNCTION()
	void OnGoToMonsterButtonClicked();

	UFUNCTION()
	void OnGoToPlayerButtonClicked();

	UFUNCTION(Server, Reliable)
	void RegisterPlayerDataToGameState(const FCustomPlayerData& CustomPlayerData);
	
protected:
	AMyLobbyPlayerController();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LobbyManagementWidgetClass = nullptr;

public:
	UFUNCTION(Client, Reliable)
	void DestroySessionOnClient();

	UFUNCTION(Client, Reliable)
	void DisplayLobbyInfoOnClient(const TArray<FCustomPlayerData>& PlayerDataList, const FString& SessionName, int MaxPlayerConnectionCount, int MaxMonsterCount);
};

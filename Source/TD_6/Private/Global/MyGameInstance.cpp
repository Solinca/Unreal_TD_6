#include "Global/MyGameInstance.h"
#include "GameFramework/PlayerState.h"

void UMyGameInstance::StartGameInstance()
{
	Super::StartGameInstance();

	CustomPlayerData.CustomPlayerID = GetFirstLocalPlayerController()->GetPlayerState<APlayerState>()->GetUniqueId();
}

void UMyGameInstance::SetCustomPlayerName(const FString& NewPlayerName)
{
	CustomPlayerData.CustomPlayerName = NewPlayerName;
}

void UMyGameInstance::SetPlayerData(FCustomPlayerData PlayerData)
{
	CustomPlayerData = PlayerData;
}

#include "Player/MyLobbyPlayerController.h"
#include "Network/OnlineSessionSubsystem.h"
#include "UI/LobbyManagementWidget.h"

AMyLobbyPlayerController::AMyLobbyPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

// TODO: Faire un RPC serveur pour faire le Client Travel 

void AMyLobbyPlayerController::OnStartButtonClicked()
{
	if (HasAuthority())
	{
		ClientTravel("/Game/Levels/BaseLevel", ETravelType::TRAVEL_Absolute);
	}
}

void AMyLobbyPlayerController::OnGoToMonsterButtonClicked()
{

}

void AMyLobbyPlayerController::OnGoToPlayerButtonClicked()
{

}

// TODO: Il me semble que le Destroy Session est mal fait et empêche la future création d'un nouveau Lobby

void AMyLobbyPlayerController::DestroySessionOnClient_Implementation()
{
	GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>()->DestroySession();

	ClientTravel("/Game/Levels/MainMenu", ETravelType::TRAVEL_Absolute);
}

void AMyLobbyPlayerController::DisplayLobbyInfoOnClient_Implementation(const TArray<class AMyPlayerState*>& PlayerList, const FString& SessionName, int MaxPlayerConnectionCount, int MaxMonsterCount)
{
	if (!LobbyManagementWidget)
	{
		LobbyManagementWidget = CreateWidget<ULobbyManagementWidget>(this, LobbyManagementWidgetClass);

		LobbyManagementWidget->AddToViewport();

		LobbyManagementWidget->OnStartButtonClicked.AddDynamic(this, &AMyLobbyPlayerController::OnStartButtonClicked);

		LobbyManagementWidget->OnGoToMonsterButtonClicked.AddDynamic(this, &AMyLobbyPlayerController::OnGoToMonsterButtonClicked);

		LobbyManagementWidget->OnGoToPlayerButtonClicked.AddDynamic(this, &AMyLobbyPlayerController::OnGoToPlayerButtonClicked);

		SetShowMouseCursor(true);

		SetInputMode(UIOnly);
	}

	LobbyManagementWidget->SetupLobby(PlayerList, SessionName, MaxPlayerConnectionCount, MaxMonsterCount, HasAuthority());
}

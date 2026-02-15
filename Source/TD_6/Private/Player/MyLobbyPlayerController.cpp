#include "Player/MyLobbyPlayerController.h"

#include "Global/MyPlayerState.h"
#include "Network/OnlineSessionSubsystem.h"
#include "UI/LobbyManagementWidget.h"

AMyLobbyPlayerController::AMyLobbyPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	const UOnlineSessionSubsystem* OnlineSessionSubsystem = GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>();

	if (!OnlineSessionSubsystem->DesiredPlayerName.IsEmpty())
	{
		ServerChangeName(OnlineSessionSubsystem->DesiredPlayerName);
	}
}

void AMyLobbyPlayerController::OnStartButtonClicked()
{
	if (HasAuthority())
	{
		GetWorld()->ServerTravel("/Game/Levels/BaseLevel?Listen");
	}
}

void AMyLobbyPlayerController::OnGoToMonsterButtonClicked()
{

}

void AMyLobbyPlayerController::OnGoToPlayerButtonClicked()
{

}

// TODO: Il me semble que le Destroy Session est mal fait et emp�che la future cr�ation d'un nouveau Lobby

void AMyLobbyPlayerController::DestroySessionOnClient_Implementation()
{
	GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>()->DestroySession();

	ClientTravel("/Game/Levels/MainMenu", ETravelType::TRAVEL_Absolute);
}

void AMyLobbyPlayerController::DisplayLobbyInfoOnClient_Implementation(const TArray<FCustomPlayerData>& PlayerDataList, const FString& SessionName, int MaxPlayerConnectionCount, int MaxMonsterCount)
{
	if (GetWorld()->bIsTearingDown)
	{
		return;
	}

	const UOnlineSessionSubsystem* OnlineSessionSubsystem = GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>();

	// if (!OnlineSessionSubsystem->DesiredPlayerName.IsEmpty())
	// {
	// 	AMyPlayerState* LocalMyPlayerState = GetPlayerState<AMyPlayerState>();
	//
	// 	for (AMyPlayerState* MyPlayerState : PlayerList)
	// 	{
	// 		if (MyPlayerState && MyPlayerState == LocalMyPlayerState)
	// 		{
	// 			MyPlayerState->SetPlayerName(OnlineSessionSubsystem->DesiredPlayerName);
	// 			break;
	// 		}
	// 	}
	// }

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

	LobbyManagementWidget->SetupLobby(PlayerDataList, SessionName, MaxPlayerConnectionCount, MaxMonsterCount, HasAuthority());
}

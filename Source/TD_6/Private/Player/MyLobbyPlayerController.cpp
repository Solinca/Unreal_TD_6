#include "Player/MyLobbyPlayerController.h"
#include "Global/MyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Network/OnlineSessionSubsystem.h"
#include "UI/LobbyManagementWidget.h"

AMyLobbyPlayerController::AMyLobbyPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		RegisterPlayerDataToGameState(GetGameInstance<UMyGameInstance>()->GetCustomPlayerData());
	}
}

void AMyLobbyPlayerController::RegisterPlayerDataToGameState_Implementation(const FCustomPlayerData& CustomPlayerData)
{
	Cast<AMyGameState>(UGameplayStatics::GetGameState(this))->RegisterPlayerData(CustomPlayerData);
}

void AMyLobbyPlayerController::ToggleStartButtonEnabled() const
{
	LobbyManagementWidget->ToggleStartButtonEnabled();
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

void AMyLobbyPlayerController::OnBackButtonClicked()
{
	DestroySessionOnClient();
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

	if (!LobbyManagementWidget)
	{
		LobbyManagementWidget = CreateWidget<ULobbyManagementWidget>(this, LobbyManagementWidgetClass);

		LobbyManagementWidget->AddToViewport();

		LobbyManagementWidget->OnStartButtonClicked.AddDynamic(this, &AMyLobbyPlayerController::OnStartButtonClicked);

		LobbyManagementWidget->OnGoToMonsterButtonClicked.AddDynamic(this, &AMyLobbyPlayerController::OnGoToMonsterButtonClicked);

		LobbyManagementWidget->OnGoToPlayerButtonClicked.AddDynamic(this, &AMyLobbyPlayerController::OnGoToPlayerButtonClicked);

		LobbyManagementWidget->OnBackButtonClicked.AddDynamic(this, &ThisClass::AMyLobbyPlayerController::OnBackButtonClicked);
		
		SetShowMouseCursor(true);

		SetInputMode(UIOnly);
	}

	LobbyManagementWidget->SetupLobby(PlayerDataList, SessionName, MaxPlayerConnectionCount, MaxMonsterCount, HasAuthority());
}

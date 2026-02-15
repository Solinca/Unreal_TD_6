#include "Player/MyMenuPlayerController.h"
#include "Network/OnlineSessionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MainMenuWidget.h"
#include "UI/LobbySelectionWidget.h"
#include "UI/SessionCreationWidget.h"

AMyMenuPlayerController::AMyMenuPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	OnlineSessionSubsystem = GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>();

	OnlineSessionSubsystem->OnFindGameSessionComplete.AddDynamic(this, &AMyMenuPlayerController::OnFindGameSessionComplete);

	MainMenuWidget = CreateWidget<UMainMenuWidget>(this, MainMenuWidgetClass);

	MainMenuWidget->AddToViewport();

	MainMenuWidget->OnPlayButtonClicked.AddDynamic(this, &AMyMenuPlayerController::OnPlayButtonClicked);

	MainMenuWidget->OnQuitButtonClicked.AddDynamic(this, &AMyMenuPlayerController::OnQuitButtonClicked);

	LobbySelectionWidget = CreateWidget<ULobbySelectionWidget>(this, LobbySelectionWidgetClass);

	LobbySelectionWidget->SetVisibility(ESlateVisibility::Hidden);

	LobbySelectionWidget->AddToViewport();

	LobbySelectionWidget->OnCreateButtonClicked.AddDynamic(this, &AMyMenuPlayerController::OnCreateLobbyButtonClicked);

	LobbySelectionWidget->OnJoinButtonClicked.AddDynamic(this, &AMyMenuPlayerController::OnJoinLobbyButtonClicked);

	LobbySelectionWidget->OnRefreshButtonClicked.AddDynamic(this, &AMyMenuPlayerController::OnRefreshLobbyButtonClicked);

	LobbySelectionWidget->OnLobbySelected.AddDynamic(this, &AMyMenuPlayerController::OnLobbySelected);

	SessionCreationWidget = CreateWidget<USessionCreationWidget>(this, SessionCreationWidgetClass);
	SessionCreationWidget->SetVisibility(ESlateVisibility::Hidden);
	SessionCreationWidget->AddToViewport();
	SessionCreationWidget->OnSessionCreationConfirmed.AddDynamic(this, &AMyMenuPlayerController::OnSessionCreationConfirmed);
	SessionCreationWidget->OnSessionCreationCancelled.AddDynamic(this, &AMyMenuPlayerController::OnSessionCreationCancelled);

	SetShowMouseCursor(true);

	SetInputMode(UIOnly);
}

void AMyMenuPlayerController::OnPlayButtonClicked()
{
	MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);

	LobbySelectionWidget->SetVisibility(ESlateVisibility::Visible);

	OnlineSessionSubsystem->FindSession(10, true);
}

void AMyMenuPlayerController::OnFindGameSessionComplete(const TArray<FCustomSessionInfo>& SessionResults, bool HasSuccess)
{
	if (HasSuccess)
	{
		LobbySelectionWidget->SetLobbyList(SessionResults);
	}
}

void AMyMenuPlayerController::OnQuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, true);
}

void AMyMenuPlayerController::OnCreateLobbyButtonClicked()
{
	LobbySelectionWidget->SetVisibility(ESlateVisibility::Hidden);

	SessionCreationWidget->SetVisibility(ESlateVisibility::Visible);
	SessionCreationWidget->ResetToDefaults();
}

void AMyMenuPlayerController::OnJoinLobbyButtonClicked()
{
	OnlineSessionSubsystem->CustomJoinSession(CurrentlySelectedLobbyID);
}

void AMyMenuPlayerController::OnRefreshLobbyButtonClicked()
{
	OnlineSessionSubsystem->FindSession(10, true);
}

void AMyMenuPlayerController::OnLobbySelected(int LobbyID)
{
	CurrentlySelectedLobbyID = LobbyID;
}

void AMyMenuPlayerController::OnSessionCreationConfirmed(const FString& SessionName, const FString& Username, int32 MaxPlayers, int32 MaxMonsters, bool IsLan)
{
	OnlineSessionSubsystem->DesiredPlayerName = Username;

	OnlineSessionSubsystem->CreateSession(SessionName, MaxPlayers, MaxMonsters, IsLan);
}

void AMyMenuPlayerController::OnSessionCreationCancelled()
{
	SessionCreationWidget->SetVisibility(ESlateVisibility::Hidden);

	LobbySelectionWidget->SetVisibility(ESlateVisibility::Visible);
}

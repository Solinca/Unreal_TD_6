#include "Player/MyMenuPlayerController.h"
#include "Network/OnlineSessionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MainMenuWidget.h"
#include "UI/LobbySelectionWidget.h"

AMyMenuPlayerController::AMyMenuPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

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

	SetShowMouseCursor(true);

	SetInputMode(UIOnly);
}

void AMyMenuPlayerController::OnPlayButtonClicked()
{
	MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);

	LobbySelectionWidget->SetVisibility(ESlateVisibility::Visible);

	// TODO: Get Lobby List with FindSessions, format it and pass it to SetLobbyList
	LobbySelectionWidget->SetLobbyList();
}

void AMyMenuPlayerController::OnQuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, true);
}

void AMyMenuPlayerController::OnCreateLobbyButtonClicked()
{

}

void AMyMenuPlayerController::OnJoinLobbyButtonClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Trying to join lobby " + FString::FromInt(CurrentlySelectedLobbyID));
}

void AMyMenuPlayerController::OnRefreshLobbyButtonClicked()
{

}

void AMyMenuPlayerController::OnLobbySelected(int LobbyID)
{
	CurrentlySelectedLobbyID = LobbyID;
}

void AMyMenuPlayerController::DestroySessionOnClient_Implementation()
{
	GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>()->DestroySession();

	ClientTravel("/Game/Levels/MainMenu", ETravelType::TRAVEL_Absolute);
}

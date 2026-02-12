#include "Player/MyLobbyPlayerController.h"
#include "Network/OnlineSessionSubsystem.h"
#include "UI/LobbyManagementWidget.h"
#include "Kismet/GameplayStatics.h"

AMyLobbyPlayerController::AMyLobbyPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CreateLobbyManagementWidget();
}

void AMyLobbyPlayerController::CreateLobbyManagementWidget_Implementation()
{
	LobbyManagementWidget = CreateWidget<ULobbyManagementWidget>(this, LobbyManagementWidgetClass);

	LobbyManagementWidget->AddToViewport();

	LobbyManagementWidget->OnStartButtonClicked.AddDynamic(this, &AMyLobbyPlayerController::OnStartButtonClicked);

	LobbyManagementWidget->OnGoToMonsterButtonClicked.AddDynamic(this, &AMyLobbyPlayerController::OnGoToMonsterButtonClicked);

	LobbyManagementWidget->OnGoToPlayerButtonClicked.AddDynamic(this, &AMyLobbyPlayerController::OnGoToPlayerButtonClicked);



	LobbyManagementWidget->SetupLobby(HasAuthority());

	SetShowMouseCursor(true);

	SetInputMode(UIOnly);
}

void AMyLobbyPlayerController::OnStartButtonClicked()
{
	ClientTravel("/Game/Levels/BaseLevel", ETravelType::TRAVEL_Absolute);
}

void AMyLobbyPlayerController::OnGoToMonsterButtonClicked()
{

}

void AMyLobbyPlayerController::OnGoToPlayerButtonClicked()
{

}

void AMyLobbyPlayerController::DestroySessionOnClient_Implementation()
{
	GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>()->DestroySession();

	ClientTravel("/Game/Levels/MainMenu", ETravelType::TRAVEL_Absolute);
}

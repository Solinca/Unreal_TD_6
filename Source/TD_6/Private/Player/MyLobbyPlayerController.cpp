#include "Player/MyLobbyPlayerController.h"
#include "Animation/WidgetAnimation.h"
#include "Global/MyLobbyGameState.h"
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
		CurrentPlayerID = GetGameInstance<UMyGameInstance>()->GetCustomPlayerData().CustomPlayerID;

		RegisterPlayerDataToGameState(GetGameInstance<UMyGameInstance>()->GetCustomPlayerData());
	}
}

void AMyLobbyPlayerController::RegisterPlayerDataToGameState_Implementation(const FCustomPlayerData& CustomPlayerData)
{
	Cast<AMyLobbyGameState>(UGameplayStatics::GetGameState(this))->RegisterPlayerData(CustomPlayerData);
}

void AMyLobbyPlayerController::OnStartButtonClicked()
{
	if (HasAuthority() && !IsLobbyStarting)
	{
		RequestStartingGame();
	}
}

void AMyLobbyPlayerController::RequestStartingGame_Implementation()
{
	Cast<AMyLobbyGameState>(UGameplayStatics::GetGameState(this))->StartLobbyIfReady();
}

void AMyLobbyPlayerController::TriggerLobbyAnimation_Implementation()
{
	IsLobbyStarting = true;

	LobbyManagementWidget->StartFadeAnimation();
}

void AMyLobbyPlayerController::OnGoToMonsterButtonClicked()
{
	if (!IsLobbyStarting)
	{
		ChangePlayerCurrentTeam(ETeam::MONSTER);
	}
}

void AMyLobbyPlayerController::OnGoToPlayerButtonClicked()
{
	if (!IsLobbyStarting)
	{
		ChangePlayerCurrentTeam(ETeam::PLAYER);
	}
}

void AMyLobbyPlayerController::ChangePlayerCurrentTeam_Implementation(ETeam NewTeam)
{
	Cast<AMyLobbyGameState>(UGameplayStatics::GetGameState(this))->ChangePlayerCurrentTeam(this, NewTeam);
}

void AMyLobbyPlayerController::OnLobbyManagementFadeFinished()
{
	if (HasAuthority() && IsLobbyStarting)
	{
		GetWorld()->ServerTravel("/Game/Levels/BaseLevel?Listen");
	}
}

void AMyLobbyPlayerController::OnSetMonsterButtonClicked(EMonsterType MonsterType)
{
	if (!IsLobbyStarting)
	{
		ChangePlayerMonsterType(MonsterType);
	}
}

void AMyLobbyPlayerController::ChangePlayerMonsterType_Implementation(EMonsterType MonsterType)
{
	Cast<AMyLobbyGameState>(UGameplayStatics::GetGameState(this))->ChangePlayerMonsterType(this, MonsterType);
}

void AMyLobbyPlayerController::OnBackButtonClicked()
{
	if (IsLobbyStarting)
	{
		return;
	}

	if (HasAuthority())
	{
		Cast<AMyLobbyGameState>(UGameplayStatics::GetGameState(this))->DestroyGame();
	}
	else
	{
		DestroySessionOnClient();
	}
}

void AMyLobbyPlayerController::DestroySessionOnClient_Implementation()
{
	GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>()->DestroySession();

	ClientTravel("/Game/Levels/MainMenu", ETravelType::TRAVEL_Absolute);
}

void AMyLobbyPlayerController::DisplayLobbyInfoOnClient_Implementation(const TArray<FCustomPlayerData>& PlayerDataList, const FString& SessionName, int MaxPlayerConnectionCount, int MaxMonsterCount)
{
	if (!LobbyManagementWidget)
	{
		LobbyManagementWidget = CreateWidget<ULobbyManagementWidget>(this, LobbyManagementWidgetClass);

		LobbyManagementWidget->AddToViewport();

		LobbyManagementWidget->OnStartButtonClicked.AddDynamic(this, &AMyLobbyPlayerController::OnStartButtonClicked);

		LobbyManagementWidget->OnGoToMonsterButtonClicked.AddDynamic(this, &AMyLobbyPlayerController::OnGoToMonsterButtonClicked);

		LobbyManagementWidget->OnGoToPlayerButtonClicked.AddDynamic(this, &AMyLobbyPlayerController::OnGoToPlayerButtonClicked);

		LobbyManagementWidget->OnBackButtonClicked.AddDynamic(this, &AMyLobbyPlayerController::OnBackButtonClicked);

		LobbyManagementWidget->OnLobbyManagementFadeFinished.AddDynamic(this, &AMyLobbyPlayerController::OnLobbyManagementFadeFinished);

		LobbyManagementWidget->OnSetMonsterButtonClicked.AddDynamic(this, &AMyLobbyPlayerController::OnSetMonsterButtonClicked);
		
		SetShowMouseCursor(true);

		SetInputMode(UIOnly);
	}

	LobbyManagementWidget->UpdateLobby(PlayerDataList, SessionName, MaxPlayerConnectionCount, MaxMonsterCount, HasAuthority(), CurrentPlayerID);
}

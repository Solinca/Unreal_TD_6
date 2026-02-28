#include "Global/MyLobbyGameMode.h"
#include "Global/MyLobbyGameState.h"
#include "Network/MyGameSession.h"
#include "Network/MyOnlineBeaconHostObject.h"
#include "GameFramework/GameSession.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineBeaconHost.h"

void AMyLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	CreateHostBeacon(7787, true);

	SetupGameSession();
}

void AMyLobbyGameMode::CreateHostBeacon(int32 ListenPort, bool bOverridePort)
{
	BeaconHost = GetWorld()->SpawnActor<AOnlineBeaconHost>();

	if (BeaconHost->InitHost())
	{
		BeaconHost->PauseBeaconRequests(false);

		if (HostObject = GetWorld()->SpawnActor<AMyOnlineBeaconHostObject>())
		{
			HostObject->ReservedSlots++;

			HostObject->MaxSlots = Cast<AMyGameSession>(GameSession)->MaxPlayerConnectionAmount;

			BeaconHost->RegisterHost(HostObject);
		}
	}
}

void AMyLobbyGameMode::SetupGameSession()
{
	FNamedOnlineSession* CurrentSession = IOnlineSubsystem::Get()->GetSessionInterface()->GetNamedSession(NAME_GameSession);

	AMyGameSession* MyGameSession = Cast<AMyGameSession>(GameSession);

	if (MyGameSession && GetGameState<AMyLobbyGameState>())
	{
		GetGameState<AMyLobbyGameState>()->SetupCurrentSession(MyGameSession->SessionName, MyGameSession->MaxPlayerConnectionAmount, MyGameSession->MaxMonsterAmount);
	}
}

void AMyLobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	if (GameSession->AtCapacity(false))
	{
		ErrorMessage = TEXT("SESSION FULL");
	}

	FNamedOnlineSession* CurrentSession = IOnlineSubsystem::Get()->GetSessionInterface()->GetNamedSession(NAME_GameSession);

	AMyGameSession* MyGameSession = Cast<AMyGameSession>(GameSession);

	if (MyGameSession && MyGameSession->SessionStatus == ESessionStatus::LAUNCHED)
	{
		ErrorMessage = TEXT("SESSION HAS ALREADY LAUNCHED");
	}
}

void AMyLobbyGameMode::Logout(AController* Controller)
{
	Super::Logout(Controller);

	if (!GetWorld() || GetWorld()->bIsTearingDown || !GetWorld()->NextURL.IsEmpty())
	{
		return;
	}

	HostObject->ReservedSlots--;

	GetGameState<AMyLobbyGameState>()->RemovePlayerData(Controller);
}

void AMyLobbyGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FNamedOnlineSession* CurrentSession = IOnlineSubsystem::Get()->GetSessionInterface()->GetNamedSession(NAME_GameSession);

	if (AMyGameSession* MyGameSession = Cast<AMyGameSession>(GameSession))
	{
		MyGameSession->MaxPlayerConnectionAmount = CurrentSession->SessionSettings.NumPublicConnections;

		CurrentSession->SessionSettings.Get("SETTING_SESSION_NAME", MyGameSession->SessionName);

		CurrentSession->SessionSettings.Get("SETTING_SESSION_MAX_MONSTER_AMOUNT", MyGameSession->MaxMonsterAmount);
	}
}

void AMyLobbyGameMode::SetCurrentSessionStatusToLaunched()
{
	FNamedOnlineSession* CurrentSession = IOnlineSubsystem::Get()->GetSessionInterface()->GetNamedSession(NAME_GameSession);

	if (AMyGameSession* MyGameSession = Cast<AMyGameSession>(GameSession))
	{
		MyGameSession->SessionStatus = ESessionStatus::LAUNCHED;

		CurrentSession->SessionSettings.Set("SETTING_SESSION_STATUS", (int) MyGameSession->SessionStatus);

		BeaconHost->PauseBeaconRequests(true);
	}
}

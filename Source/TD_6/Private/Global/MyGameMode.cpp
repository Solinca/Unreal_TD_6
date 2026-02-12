#include "Global/MyGameMode.h"
#include "Global/MyGameState.h"
#include "Network/MyGameSession.h"
#include "Network/MyOnlineBeaconHostObject.h"
#include "Player/MyLobbyPlayerController.h"
#include "GameFramework/GameSession.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineBeaconHost.h"

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();

	CreateHostBeacon(7787, true);
}

void AMyGameMode::CreateHostBeacon(int32 ListenPort, bool bOverridePort)
{
	AOnlineBeaconHost* BeaconHost = GetWorld()->SpawnActor<AOnlineBeaconHost>();

	if (BeaconHost->InitHost())
	{
		BeaconHost->PauseBeaconRequests(false);

		if (AMyOnlineBeaconHostObject* HostObject = GetWorld()->SpawnActor<AMyOnlineBeaconHostObject>())
		{
			HostObject->ReservedSlots++;

			HostObject->MaxSlots = Cast<AMyGameSession>(GameSession)->MaxPlayerConnectionAmount;

			BeaconHost->RegisterHost(HostObject);
		}
	}
}

void AMyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	if (GameSession->AtCapacity(false))
	{
		ErrorMessage = TEXT("SESSION FULL");
	}
}

void AMyGameMode::OnPostLogin(AController* Controller)
{
	Super::OnPostLogin(Controller);

	AMyGameSession* MyGameSession = Cast<AMyGameSession>(GameSession);

	if (MyGameSession)
	{
		GetGameState<AMyGameState>()->SetupCurrentSession(MyGameSession->SessionName, MyGameSession->MaxPlayerConnectionAmount, MyGameSession->MaxMonsterAmount);

		GetGameState<AMyGameState>()->PlayerJoined(Controller);
	}
}

void AMyGameMode::Logout(AController* Controller)
{
	Super::Logout(Controller);

	GetGameState<AMyGameState>()->PlayerLeft(Controller);
}

void AMyGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	IOnlineSessionPtr SessionInterface = IOnlineSubsystem::Get()->GetSessionInterface();

	FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(NAME_GameSession);

	AMyGameSession* MyGameSession = Cast<AMyGameSession>(GameSession);

	if (CurrentSession && MyGameSession)
	{
		MyGameSession->MaxPlayerConnectionAmount = CurrentSession->SessionSettings.NumPublicConnections;

		CurrentSession->SessionSettings.Get("SETTING_SESSION_NAME", MyGameSession->SessionName);

		CurrentSession->SessionSettings.Get("SETTING_SESSION_MAX_MONSTER_AMOUNT", MyGameSession->MaxMonsterAmount);
	}
}

void AMyGameMode::DestroyGame()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		if (AMyLobbyPlayerController* PC = Cast<AMyLobbyPlayerController>(It->Get()))
		{
			PC->DestroySessionOnClient();
		}
	}
}

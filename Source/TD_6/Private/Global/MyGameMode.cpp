#include "Global/MyGameMode.h"
#include "Global/MyGameState.h"
#include "Network/MyGameSession.h"
#include "Network/MyOnlineBeaconHostObject.h"
#include "Network/OnlineSessionSubsystem.h"
#include "Player/MyLobbyPlayerController.h"
#include "GameFramework/GameSession.h"
#include "OnlineBeaconHost.h"

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();

	CreateHostBeacon(7787, true);

	SetupGameSession();
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

void AMyGameMode::SetupGameSession()
{
	IOnlineSessionPtr SessionInterface = IOnlineSubsystem::Get()->GetSessionInterface();

	FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(NAME_GameSession);

	AMyGameSession* MyGameSession = Cast<AMyGameSession>(GameSession);

	if (MyGameSession && GetGameState<AMyGameState>())
	{
		GetGameState<AMyGameState>()->SetupCurrentSession(MyGameSession->SessionName, MyGameSession->MaxPlayerConnectionAmount, MyGameSession->MaxMonsterAmount);
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

	GetGameState<AMyGameState>()->PlayerJoined(Controller);
}

void AMyGameMode::Logout(AController* Controller)
{
	Super::Logout(Controller);

	GetGameState<AMyGameState>()->PlayerLeft(Controller);
}

void AMyGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	AMyGameSession* MyGameSession = Cast<AMyGameSession>(GameSession);

	if (!MyGameSession)
	{
		return;
	}

	const UOnlineSessionSubsystem* OnlineSessionSubsystem = GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>();

	if (OnlineSessionSubsystem)
	{
		MyGameSession->MaxPlayerConnectionAmount = OnlineSessionSubsystem->MaxPlayerCount;
		MyGameSession->SessionName = OnlineSessionSubsystem->DesiredSessionName;
		MyGameSession->MaxMonsterAmount = OnlineSessionSubsystem->MaxMonsterCount;
	}
}

void AMyGameMode::ChangeName(AController* Controller, const FString& NewName, bool bNameChange)
{
	Super::ChangeName(Controller, NewName, bNameChange);

	GetGameState<AMyGameState>()->DisplayEveryPlayer();
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

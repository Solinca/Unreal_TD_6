#include "Network/OnlineSessionSubsystem.h"
#include "Network/MyOnlineBeaconClient.h"
#include "Network/MyGameSession.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSubsystemUtils.h"

void UOnlineSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Session = Online::GetSessionInterface(GetWorld());
}

void UOnlineSessionSubsystem::OnCreateSessionCompleted(FName SessionName, bool IsSuccessful)
{
	if (Session)
	{
		Session->ClearOnCreateSessionCompleteDelegate_Handle(CreateHandle);
	}

	if (!IsSuccessful)
	{
		return;
	}

	GetWorld()->ServerTravel("/Game/Levels/LobbyManagement?Listen");
}

void UOnlineSessionSubsystem::OnFindSessionCompleted(bool IsSuccessful)
{
	if (Session)
	{
		Session->ClearOnFindSessionsCompleteDelegate_Handle(FindHandle);
	}

	SearchResults = LastSessionSearch->SearchResults;

	if (SearchResults.IsEmpty())
	{
		OnFindGameSessionComplete.Broadcast(TArray<FCustomSessionInfo>(), IsSuccessful);

		return;
	}

	TArray<FCustomSessionInfo> SessionsInfo;

	for (int i = 0; i < SearchResults.Num(); i++)
	{
		FOnlineSessionSearchResult Result = SearchResults[i];

		FCustomSessionInfo SessionInfo;

		int CurrentSessionStatus;

		Result.Session.SessionSettings.Get("SETTING_SESSION_STATUS", CurrentSessionStatus);

		if (CurrentSessionStatus == ESessionStatus::LAUNCHED)
		{
			continue;
		}

		FString SessionName;

		Result.Session.SessionSettings.Get("SETTING_SESSION_NAME", SessionName);

		SessionInfo.SessionName = SessionName;

		SessionInfo.MaxPlayerConnectionAmount = Result.Session.SessionSettings.NumPublicConnections;

		SessionInfo.SessionSearchResultIndex = i;

		SessionInfo.CurrentPlayerCount = Result.Session.SessionSettings.NumPublicConnections - Result.Session.NumOpenPublicConnections;

		SessionsInfo.Add(SessionInfo);
	}

	OnFindGameSessionComplete.Broadcast(SessionsInfo, IsSuccessful);
}

void UOnlineSessionSubsystem::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	FString ConnectString;

	if (Session)
	{
		Session->ClearOnJoinSessionCompleteDelegate_Handle(JoinHandle);
	}

	if (Result != EOnJoinSessionCompleteResult::Success || !Session->GetResolvedConnectString(NAME_GameSession, ConnectString))
	{
		return;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	PlayerController->ClientTravel(ConnectString, TRAVEL_Absolute);
}

void UOnlineSessionSubsystem::OnDestroySessionCompleted(FName SessionName, bool IsSuccessful)
{
	if (Session)
	{
		Session->ClearOnDestroySessionCompleteDelegate_Handle(DestroyHandle);
	}
}

void UOnlineSessionSubsystem::OnUpdatedSessionSettingsCompleted(FName SessionName, bool IsSuccessful)
{
	if (Session)
	{
		Session->ClearOnUpdateSessionCompleteDelegate_Handle(UpdateHandle);
	}
}

void UOnlineSessionSubsystem::JoinGameSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!Session.IsValid())
	{
		return;
	}

	JoinHandle = Session->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UOnlineSessionSubsystem::OnJoinSessionCompleted));

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	if (!Session->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		Session->ClearOnJoinSessionCompleteDelegate_Handle(JoinHandle);

		return;
	}
}

void UOnlineSessionSubsystem::CreateSession(const FString& SessionName, int32 NumPublicConnections, int32 MaxMonsterCount, bool IsLanMatch)
{
	if (!Session.IsValid())
	{
		return;
	}

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());

	LastSessionSettings->NumPublicConnections = NumPublicConnections;

	LastSessionSettings->bAllowJoinInProgress = true;

	LastSessionSettings->bAllowJoinViaPresence = true;

	LastSessionSettings->bIsDedicated = false;

	LastSessionSettings->bUsesPresence = true;

	LastSessionSettings->bIsLANMatch = IsLanMatch;

	LastSessionSettings->bShouldAdvertise = true;

	LastSessionSettings->Set("SETTING_SESSION_NAME", SessionName, EOnlineDataAdvertisementType::ViaOnlineService);

	LastSessionSettings->Set("SETTING_SESSION_MAX_MONSTER_AMOUNT", MaxMonsterCount, EOnlineDataAdvertisementType::ViaOnlineService);

	LastSessionSettings->Set("SETTING_SESSION_STATUS", (int) ESessionStatus::PREPARING, EOnlineDataAdvertisementType::ViaOnlineService);

	CreateHandle = Session->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UOnlineSessionSubsystem::OnCreateSessionCompleted));

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	if (!Session->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		Session->ClearOnCreateSessionCompleteDelegate_Handle(CreateHandle);
	}
}

void UOnlineSessionSubsystem::FindSession(int32 MaxSearchResults, bool IsLanQuery)
{
	if (!Session.IsValid())
	{
		return;
	}

	FindHandle = Session->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UOnlineSessionSubsystem::OnFindSessionCompleted));

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());

	LastSessionSearch->MaxSearchResults = MaxSearchResults;

	LastSessionSearch->bIsLanQuery = IsLanQuery;

	LastSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	if (!Session->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		Session->ClearOnFindSessionsCompleteDelegate_Handle(FindHandle);
	}
}

void UOnlineSessionSubsystem::CustomJoinSession(int32 SessionIndex)
{
	const FOnlineSessionSearchResult TempResult = SearchResults[SessionIndex];

	FString ConnectString;

	if (Session->GetResolvedConnectString(TempResult, NAME_BeaconPort, ConnectString))
	{
		AMyOnlineBeaconClient* BeaconClient = GetWorld()->SpawnActor<AMyOnlineBeaconClient>();

		FURL Destination = FURL(nullptr, *ConnectString, ETravelType::TRAVEL_Absolute);

		Destination.Port = 7787;

		BeaconClient->OnRequestValidate.BindLambda([this, TempResult](bool IsValid)
		{
			HasHandshakeCompleted = true;

			if (IsValid)
			{
				JoinGameSession(TempResult);
			}
			else
			{
				OnJoinSessionFailed.Broadcast("Someone took your spot! \n The room is full :/");
			}
		});

		BeaconClient->InitClient(Destination);

		FTimerHandle WaitForHandshakeHandle;

		GetWorld()->GetTimerManager().SetTimer(WaitForHandshakeHandle, [this]
		{
			if (!HasHandshakeCompleted)
			{
				OnJoinSessionFailed.Broadcast("Session doesn't exist anymore \n or has already launched :(");
			}
		}, 10, false);
	}
	else
	{
		OnJoinSessionFailed.Broadcast("Can't connect to server :O");
	}
}

void UOnlineSessionSubsystem::DestroySession()
{
	if (!Session.IsValid())
	{
		return;
	}

	DestroyHandle = Session->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UOnlineSessionSubsystem::OnDestroySessionCompleted));

	if (!Session->DestroySession(NAME_GameSession))
	{
		Session->ClearOnDestroySessionCompleteDelegate_Handle(DestroyHandle);
	}
}

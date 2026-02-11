#include "Network/OnlineSessionSubsystem.h"
#include "Network/MyOnlineBeaconHostObject.h"
#include "Network/MyOnlineBeaconClient.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineBeaconHost.h"

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

	/*
	* UPDATE SESSION SETTINGS AFTER 10 SECONDS
	* 
	FTimerHandle UpdateSessionSettingsHandle;

	GetWorld()->GetTimerManager().SetTimer(UpdateSessionSettingsHandle, [this] { UpdateCustomSessionSettings("SESSION_STATE", 1, EOnlineDataAdvertisementType::ViaOnlineService); }, 10, false);
	*/

	FTimerHandle CreateHostBeaconHandle;

	GetWorld()->GetTimerManager().SetTimer(CreateHostBeaconHandle, [this] { CreateHostBeacon(7787, true); }, 2, false);
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

		FString SessionName;

		Result.Session.SessionSettings.Get("SETTING_SESSIONNAME", SessionName);

		int SessionState;

		Result.Session.SessionSettings.Get("SESSION_STATE", SessionState);

		if (SessionState > 0)
		{
			continue;
		}

		SessionInfo.SessionName = SessionName;

		SessionInfo.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;

		SessionInfo.CurrentPlayers = Result.Session.SessionSettings.NumPublicConnections - Result.Session.NumOpenPublicConnections;

		SessionInfo.Ping = Result.PingInMs;

		SessionInfo.SessionSearchResultIndex = i;

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
		// Do something like broadcast to display join error

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

void UOnlineSessionSubsystem::CreateSession(const FString& SessionName, int32 NumPublicConnections, bool IsLanMatch)
{
	if (!Session.IsValid())
	{
		return;
	}

	MaxPlayers = NumPublicConnections;

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());

	LastSessionSettings->NumPublicConnections = NumPublicConnections;

	LastSessionSettings->bAllowJoinInProgress = true;

	LastSessionSettings->bAllowJoinViaPresence = true;

	LastSessionSettings->bIsDedicated = false;

	LastSessionSettings->bUsesPresence = true;

	LastSessionSettings->bIsLANMatch = IsLanMatch;

	LastSessionSettings->bShouldAdvertise = true;

	LastSessionSettings->Set("SETTING_SESSIONNAME", SessionName, EOnlineDataAdvertisementType::ViaOnlineService);

	LastSessionSettings->Set("SESSION_STATE", 0, EOnlineDataAdvertisementType::ViaOnlineService);

	CreateHandle = Session->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UOnlineSessionSubsystem::OnCreateSessionCompleted));

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	if (!Session->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		Session->ClearOnCreateSessionCompleteDelegate_Handle(CreateHandle);

		return;
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

		return;
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

		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Trying to connect to: %s:%d"), *Destination.Host, Destination.Port));

		BeaconClient->ConnectToServer(Destination);

		BeaconClient->OnRequestValidate.BindLambda([this, TempResult](bool IsValid)
		{
			if (IsValid)
			{
				JoinGameSession(TempResult);
			}
			else
			{
				// Display an error via broadcast

				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Failed to connect...");
			}
		});
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

		return;
	}
}

template<typename ValueType>
inline void UOnlineSessionSubsystem::UpdateCustomSessionSettings(const FName& KeyName, const ValueType& Value, EOnlineDataAdvertisementType::Type InType)
{
	if (!Session.IsValid() || !LastSessionSettings.IsValid())
	{
		return;
	}

	TSharedPtr<FOnlineSessionSettings> UpdatedSessionSettings = MakeShareable(new FOnlineSessionSettings(*LastSessionSettings));

	UpdatedSessionSettings->Set(KeyName, Value, InType);

	UpdateHandle = Session->AddOnUpdateSessionCompleteDelegate_Handle(FOnUpdateSessionCompleteDelegate::CreateUObject(this, &UOnlineSessionSubsystem::OnUpdatedSessionSettingsCompleted));

	if (!Session->UpdateSession(NAME_GameSession, *UpdatedSessionSettings))
	{
		Session->ClearOnUpdateSessionCompleteDelegate_Handle(UpdateHandle);

		return;
	}

	LastSessionSettings = UpdatedSessionSettings;

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "SESSION SETTINGS UPDATED");
}

void UOnlineSessionSubsystem::CreateHostBeacon(int32 ListenPort, bool bOverridePort)
{
	AOnlineBeaconHost* BeaconHost = GetWorld()->SpawnActor<AOnlineBeaconHost>();

	if (BeaconHost->InitHost())
	{
		BeaconHost->PauseBeaconRequests(false);

		if (AMyOnlineBeaconHostObject* HostObject = GetWorld()->SpawnActor<AMyOnlineBeaconHostObject>())
		{
			HostObject->ReservedSlots++;

			HostObject->MaxSlots = MaxPlayers;

			BeaconHost->RegisterHost(HostObject);

			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "HOST BEACON CREATED");
		}
	}
}

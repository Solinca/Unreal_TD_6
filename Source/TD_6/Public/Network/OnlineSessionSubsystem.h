#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSessionSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FCustomSessionInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString SessionName = "";

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPlayers = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Ping = 0;

	int32 SessionSearchResultIndex = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFindGameSessionCompleteSignature, const TArray<FCustomSessionInfo>&, SessionResults, bool, HasSuccess);

UCLASS()
class TD_6_API UOnlineSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	IOnlineSessionPtr Session;

	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;

	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	TArray<FOnlineSessionSearchResult> SearchResults;

	FDelegateHandle CreateHandle;

	FDelegateHandle FindHandle;

	FDelegateHandle JoinHandle;

	FDelegateHandle DestroyHandle;

	FDelegateHandle UpdateHandle;

	void OnCreateSessionCompleted(FName SessionName, bool IsSuccessful);

	void OnFindSessionCompleted(bool IsSuccessful);

	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void OnDestroySessionCompleted(FName SessionName, bool IsSuccessful);

	void OnUpdatedSessionSettingsCompleted(FName SessionName, bool IsSuccessful);

	void JoinGameSession(const FOnlineSessionSearchResult& SessionResult);

	void CreateHostBeacon(int32 ListenPort, bool bOverridePort);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Session")
	void CreateSession(const FString& SessionName, int32 NumPublicConnections, bool IsLanMatch);

	UFUNCTION(BlueprintCallable, Category = "Session")
	void FindSession(int32 MaxSearchResults, bool IsLanQuery);

	UFUNCTION(BlueprintCallable, Category = "Session")
	void CustomJoinSession(int32 SessionIndex);

	UFUNCTION(Category = "Session")
	void DestroySession();

	template<typename ValueType>
	void UpdateCustomSessionSettings(const FName& KeyName, const ValueType& Value, EOnlineDataAdvertisementType::Type InType);

	UPROPERTY(BlueprintAssignable, Category = "Session")
	FOnFindGameSessionCompleteSignature OnFindGameSessionComplete;

	int32 MaxPlayers = 0;
};

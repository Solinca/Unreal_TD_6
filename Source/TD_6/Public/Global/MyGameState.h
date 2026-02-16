#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Global/MyGameInstance.h"
#include "MyGameState.generated.h"

UCLASS()
class TD_6_API AMyGameState : public AGameState
{
	GENERATED_BODY()

private:
	UPROPERTY(Replicated)
	FString CurrentSessionName;

	UPROPERTY(Replicated)
	int32 CurrentMaxPlayerConnectionAmount;

	UPROPERTY(Replicated)
	int32 CurrentMaxMonsterAmount;

	UPROPERTY(ReplicatedUsing = DisplayEveryPlayerInLobby)
	TArray<FCustomPlayerData> PlayerDataList;

	UFUNCTION()
	void DisplayEveryPlayerInLobby();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void RegisterPlayerData(const FCustomPlayerData& CustomPlayerData);

	void RemovePlayerData(AController* Controller);

	UFUNCTION(Server, Reliable)
	void SetupCurrentSession(const FString& SessionName, int MaxConnectionAmount, int MaxMonsterAmount);

	UFUNCTION(Server, Reliable)
	void DestroyGame();
};

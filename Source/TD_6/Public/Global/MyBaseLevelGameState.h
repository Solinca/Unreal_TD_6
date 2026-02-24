#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Global/MyGameInstance.h"
#include "MyBaseLevelGameState.generated.h"

class UMonsterDataAsset;

UCLASS()
class TD_6_API AMyBaseLevelGameState : public AGameState
{
	GENERATED_BODY()

private:
	FTimerHandle GameStartCountdownHandle;

	int CurrentLoadedPlayer = 0;

	UPROPERTY(ReplicatedUsing = DisplayTimerToClients)
	int TimeToWaitBeforeGameStart = 4;

	void CountdownTimer();

	UFUNCTION()
	void DisplayTimerToClients();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	TMap<TEnumAsByte<EMonsterType>, UMonsterDataAsset*> MonsterDataPerType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Multiplayer|Settings")
	float PlayerWaitingTimeAtStart = 3.f;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	void HandlePlayer(AController* Controller);

	void RemovePlayer(AController* Controller);

	UFUNCTION(Server, Reliable)
	void PlayerHasLoaded();

	UFUNCTION(Server, Reliable)
	void DestroyGame();
};

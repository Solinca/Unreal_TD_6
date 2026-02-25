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
	TArray<class ATargetPoint*> WinningTargetPointList;

	TArray<class ATargetPoint*> LoosingTargetPointList;

	FTimerHandle GameStartCountdownHandle;

	FTimerHandle GameGlobalTimerHandle;

	int CurrentLoadedPlayer = 0;

	int CurrentNumberOfMonsterPlayer = 0;

	int CurrentNumberOfCompletedPlayerObjective = 0;

	void CountdownTimer();

	UPROPERTY(ReplicatedUsing = DisplayCountdownToClients)
	int TimeToWaitBeforeGameStart = 4;

	UFUNCTION()
	void DisplayCountdownToClients();

	void CountdownGlobalTimer();

	UPROPERTY(ReplicatedUsing = DisplayTimerToClients)
	int GlobalGameTimer = 301;

	UFUNCTION()
	void DisplayTimerToClients();

	void SetupPlayerObjectives();

	void TriggerResultScreen(ETeam WinningTeam);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	TMap<TEnumAsByte<EMonsterType>, UMonsterDataAsset*> MonsterDataPerType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Multiplayer|Settings")
	float PlayerWaitingTimeAtStart = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Multiplayer|Settings")
	TSubclassOf<AActor> PlayerObjective = nullptr;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	void HandlePlayer(AController* Controller);

	void RemovePlayer(AController* Controller);

	void RegisterPlayerObjectiveCompleted();

	UFUNCTION(Server, Reliable)
	void PlayerHasLoaded();

	UFUNCTION(Server, Reliable)
	void DestroyGame();
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
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

	UPROPERTY(ReplicatedUsing = DisplayEveryPlayer)
	TArray<class AMyPlayerState*> PlayerList;

	UFUNCTION(Client, Reliable)
	void DisplayEveryPlayer();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(Server, Reliable)
	void PlayerJoined(AController* Controller);

	UFUNCTION(Server, Reliable)
	void PlayerLeft(AController* Controller);

	UFUNCTION(Server, Reliable)
	void SetupCurrentSession(const FString& SessionName, int MaxConnectionAmount, int MaxMonsterAmount);
};

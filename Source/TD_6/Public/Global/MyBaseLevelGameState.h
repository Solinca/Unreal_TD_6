#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyBaseLevelGameState.generated.h"

UCLASS()
class TD_6_API AMyBaseLevelGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	void RemovePlayer(AController* Controller);

	UFUNCTION(Server, Reliable)
	void DestroyGame();
};

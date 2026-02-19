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

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DATA")
	TMap<TEnumAsByte<EMonsterType>, UMonsterDataAsset*> MonsterDataPerType;
	
public:
	void HandlePlayer(AController* Controller);

	void RemovePlayer(AController* Controller);

	UFUNCTION(Server, Reliable)
	void DestroyGame();

	UMonsterDataAsset* RetrieveMonsterData(EMonsterType MonsterType) { return MonsterDataPerType[MonsterType]; };
};

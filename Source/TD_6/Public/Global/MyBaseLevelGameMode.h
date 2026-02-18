#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MyBaseLevelGameMode.generated.h"

UCLASS()
class TD_6_API AMyBaseLevelGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Controller) override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* Controller) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn CLasses")
	TSubclassOf<APawn> PlayerPawnClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn CLasses")
	TSubclassOf<APawn> ButcherMonsterPawnClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn CLasses")
	TSubclassOf<APawn> HunterMonsterPawnClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn CLasses")
	TSubclassOf<APawn> SlimeMonsterPawnClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn CLasses")
	TSubclassOf<APawn> GhostMonsterPawnClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn CLasses")
	TSubclassOf<APawn> PredatorMonsterPawnClass = nullptr;
};

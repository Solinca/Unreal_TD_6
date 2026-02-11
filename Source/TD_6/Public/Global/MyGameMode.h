#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MyGameMode.generated.h"

UCLASS()
class TD_6_API AMyGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void OnPostLogin(AController* Controller) override;

	virtual void Logout(AController* Controller) override;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

public:
	UFUNCTION(BlueprintCallable)
	void DestroyGame();
};

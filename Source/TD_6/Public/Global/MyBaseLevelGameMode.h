#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MyBaseLevelGameMode.generated.h"

UCLASS()
class TD_6_API AMyBaseLevelGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:
	virtual void Logout(AController* Controller) override;
};

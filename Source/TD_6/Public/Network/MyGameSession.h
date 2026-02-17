#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "MyGameSession.generated.h"

UENUM()
enum ESessionStatus
{
	PREPARING = 0,
	LAUNCHED = 1
};

UCLASS()
class TD_6_API AMyGameSession : public AGameSession
{
	GENERATED_BODY()
	
public:
	FString SessionName;

	int32 MaxPlayerConnectionAmount;

	int32 MaxMonsterAmount;

	ESessionStatus SessionStatus = ESessionStatus::PREPARING;
};

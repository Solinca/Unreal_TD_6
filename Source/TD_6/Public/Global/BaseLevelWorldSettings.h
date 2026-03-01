#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "BaseLevelWorldSettings.generated.h"

UCLASS()
class TD_6_API ABaseLevelWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "End Screen Settings")
	TObjectPtr<AActor> EndScreenCamera = nullptr;

	UPROPERTY(EditAnywhere, Category = "End Screen Settings")
	TObjectPtr<class ALight> EndScreenLighting = nullptr;

	UPROPERTY(EditAnywhere, Category = "End Screen Settings")
	FLinearColor PlayerWinningColor;

	UPROPERTY(EditAnywhere, Category = "End Screen Settings")
	FLinearColor MonsterWinningColor;
};

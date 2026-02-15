#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

UENUM()
enum ETeam
{
	PLAYER,
	MONSTER
};

const TArray<FString> DefaultAnimalNames{ "AnonymousLama", "AnonymousGoat", "AnonymousBear", "AnonymousRabbit" };

USTRUCT()
struct FCustomPlayerData
{
	GENERATED_BODY()

	UPROPERTY()
	FString CustomPlayerName;

	UPROPERTY()
	TEnumAsByte<ETeam> CurrentTeam = ETeam::PLAYER;

	FCustomPlayerData()
	{
		FString DefaultPlayerName = DefaultAnimalNames[FMath::Rand() % 4];

		CustomPlayerName = DefaultPlayerName.Append(FString::FromInt(FMath::Rand() % 100));
	}
};

UCLASS()
class TD_6_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
private:
	UPROPERTY(Replicated)
	FCustomPlayerData CustomPlayerData;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(Server, Reliable)
	void SetCustomPlayerName(const FString& NewPlayerName);

	UFUNCTION(Server, Reliable)
	void SetCurrentTeam(ETeam NewTeam);

	FCustomPlayerData GetCustomPlayerData() { return CustomPlayerData; };
};

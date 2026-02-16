#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

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
	FUniqueNetIdRepl CustomPlayerID;

	UPROPERTY()
	FString CustomPlayerName;

	UPROPERTY()
	TEnumAsByte<ETeam> CurrentTeam = ETeam::PLAYER;

	FCustomPlayerData()
	{
		FString DefaultPlayerName = DefaultAnimalNames[FMath::Rand() % 4];

		CustomPlayerName = DefaultPlayerName.Append(FString::FromInt(FMath::Rand() % 100));
	}

	bool operator == (FCustomPlayerData& Other) const
	{
		return CustomPlayerID == Other.CustomPlayerID;
	}
};

UCLASS()
class TD_6_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
private:
	FCustomPlayerData CustomPlayerData;

protected:
	virtual void StartGameInstance() override;

public:
	void SetCustomPlayerName(const FString& NewPlayerName);

	void SetCurrentTeam(ETeam NewTeam);

	FCustomPlayerData GetCustomPlayerData() { return CustomPlayerData; };
};

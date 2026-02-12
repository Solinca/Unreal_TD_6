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

UCLASS()
class TD_6_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
private:
	ETeam CurrentTeam = ETeam::PLAYER;

public:
	void SetCurrentTeam(ETeam NewTeam) { CurrentTeam = NewTeam; };

	ETeam GetCurrentTeam() { return CurrentTeam; };
};

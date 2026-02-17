#include "Global/MyBaseLevelGameState.h"
#include "Player/MyPlayerController.h"

void AMyBaseLevelGameState::RemovePlayerData(AController* Controller)
{
	if (Controller->IsLocalController())
	{
		DestroyGame();
	}
}

void AMyBaseLevelGameState::DestroyGame_Implementation()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		if (AMyPlayerController* MyLPC = Cast<AMyPlayerController>(It->Get()))
		{
			MyLPC->DestroySessionOnClient();
		}
	}
}

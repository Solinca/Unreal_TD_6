#include "Global/MyBaseLevelGameState.h"
#include "Global/MyGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Player/MyPlayerController.h"

void AMyBaseLevelGameState::HandlePlayer(AController* Controller)
{
	Cast<AMyPlayerController>(Controller)->SetupClient(GetGameInstance<UMyGameInstance>()->RetrieveServerPlayerDataList(Controller->GetPlayerState<APlayerState>()->GetUniqueId()));
}

void AMyBaseLevelGameState::RemovePlayer(AController* Controller)
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

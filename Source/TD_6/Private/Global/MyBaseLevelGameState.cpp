#include "Global/MyBaseLevelGameState.h"
#include "GameFramework/PlayerState.h"
#include "Player/MyPlayerController.h"
#include "Data/MonsterDataAsset.h"

void AMyBaseLevelGameState::HandlePlayer(AController* Controller)
{
	Cast<AMyPlayerController>(Controller)->SetupClient(GetGameInstance<UMyGameInstance>()->RetrieveServerPlayerData(Controller->GetPlayerState<APlayerState>()->GetUniqueId()));
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

#include "Global/MyPlayerState.h"
#include "Net/UnrealNetwork.h"

void AMyPlayerState::SetCustomPlayerName(const FString& NewPlayerName)
{
	CustomPlayerData.CustomPlayerName = NewPlayerName;
}

void AMyPlayerState::SetCurrentTeam_Implementation(ETeam NewTeam)
{
	CustomPlayerData.CurrentTeam = NewTeam;
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerState, CustomPlayerData);
}

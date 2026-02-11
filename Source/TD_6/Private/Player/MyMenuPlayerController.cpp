#include "Player/MyMenuPlayerController.h"
#include "Network/OnlineSessionSubsystem.h"

void AMyMenuPlayerController::DestroySessionOnClient_Implementation()
{
	GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>()->DestroySession();

	ClientTravel("/Game/Levels/MainMenu", ETravelType::TRAVEL_Absolute);
}
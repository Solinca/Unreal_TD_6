#include "Network/MyOnlineBeaconHostObject.h"
#include "Network/MyOnlineBeaconClient.h"

AMyOnlineBeaconHostObject::AMyOnlineBeaconHostObject(const FObjectInitializer& Initializer) : Super(Initializer)
{
	ClientBeaconActorClass = AMyOnlineBeaconClient::StaticClass();

	BeaconTypeName = ClientBeaconActorClass->GetName();
}

void AMyOnlineBeaconHostObject::OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection)
{
	Super::OnClientConnected(NewClientActor, ClientConnection);

	if (NewClientActor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "CONNECTED CLIENT VALID");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "CONNECTED CLIENT INVALID");
	}
}

AOnlineBeaconClient* AMyOnlineBeaconHostObject::SpawnBeaconActor(UNetConnection* ClientConnection)
{
	return Super::SpawnBeaconActor(ClientConnection);
}

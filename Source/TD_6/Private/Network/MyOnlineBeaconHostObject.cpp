#include "Network/MyOnlineBeaconHostObject.h"
#include "Network/MyOnlineBeaconClient.h"

AMyOnlineBeaconHostObject::AMyOnlineBeaconHostObject(const FObjectInitializer& Initializer) : Super(Initializer)
{
	ClientBeaconActorClass = AMyOnlineBeaconClient::StaticClass();

	BeaconTypeName = ClientBeaconActorClass->GetName();
}

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconHostObject.h"
#include "MyOnlineBeaconHostObject.generated.h"

UCLASS()
class TD_6_API AMyOnlineBeaconHostObject : public AOnlineBeaconHostObject
{
	GENERATED_BODY()

public:
	AMyOnlineBeaconHostObject(const FObjectInitializer& Initializer);

	virtual void OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection) override;

	virtual AOnlineBeaconClient* SpawnBeaconActor(UNetConnection* ClientConnection) override;

	int32 ReservedSlots = 0;

	int32 MaxSlots = 2;
};

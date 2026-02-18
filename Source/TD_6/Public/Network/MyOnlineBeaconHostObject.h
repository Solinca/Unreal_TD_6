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

	int32 ReservedSlots = 0;

	int32 MaxSlots = 2;
};

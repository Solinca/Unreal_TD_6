#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyMenuPlayerController.generated.h"

UCLASS()
class TD_6_API AMyMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Client, Reliable)
	void DestroySessionOnClient();
};

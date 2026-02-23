#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class TD_6_API IInteractable
{
	GENERATED_BODY()

public:
	virtual bool InteractWith() PURE_VIRTUAL(IInteractable::CanInteract, return false;);

	virtual void StopInteractWith() PURE_VIRTUAL(IInteractable::CanInteract, );
};

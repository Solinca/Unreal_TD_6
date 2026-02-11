#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyPlayerItemWidget.generated.h"

UCLASS()
class TD_6_API ULobbyPlayerItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerNameText = nullptr;

public:
	void SetPlayerName(FString PlayerName);
};

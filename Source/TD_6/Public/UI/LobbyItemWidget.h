#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyItemWidget.generated.h"

UCLASS()
class TD_6_API ULobbyItemWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void OnSelectLobbyClickedEvent();

	TObjectPtr<class ULobbySelectionWidget> LobbySelectionWidget = nullptr;

	int CurrentLobbyID = -1;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> Background = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> SelectLobbyButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LobbyNameText = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LobbyPlayerCountText = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	FColor NormalColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	FColor SelectedColor;

public:
	void SetData(class ULobbySelectionWidget* Parent, int LobbyID, FString LobbyName, int MaxPlayerCount, int CurrentPlayerCount);

	void SetHighlight(bool IsSelected);

	int GetLobbyID() { return CurrentLobbyID; };
};

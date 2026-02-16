#include "UI/SessionCreationWidget.h"
#include "Global/MyGameInstance.h"
#include "Components/EditableText.h"
#include "Components/SpinBox.h"
#include "Components/CheckBox.h"
#include "Components/Button.h"

void USessionCreationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateButton->OnClicked.AddDynamic(this, &USessionCreationWidget::OnCreateButtonClickedEvent);
	
	BackButton->OnClicked.AddDynamic(this, &USessionCreationWidget::OnBackButtonClickedEvent);

	MaxPlayersSpinBox->SetMinValue(2);

	MaxPlayersSpinBox->SetMinSliderValue(2);

	MaxPlayersSpinBox->SetMaxValue(10);

	MaxPlayersSpinBox->SetMaxSliderValue(10);

	MaxPlayersSpinBox->SetDelta(1);

	MaxMonstersSpinBox->SetMinValue(1);

	MaxMonstersSpinBox->SetMinSliderValue(1);

	MaxMonstersSpinBox->SetMaxValue(3);

	MaxMonstersSpinBox->SetMaxSliderValue(3);

	MaxMonstersSpinBox->SetDelta(1);

	ResetToDefaults();
}

void USessionCreationWidget::OnCreateButtonClickedEvent() 
{
	const FString SessionName = SessionNameInput->GetText().ToString().TrimStartAndEnd();

	const FString Username = UsernameInput->GetText().ToString().TrimStartAndEnd();

	if (SessionName.Len() < 2 || Username.Len() < 1)
	{
		return;
	}

	const int32 MaxPlayers = FMath::RoundToInt32(MaxPlayersSpinBox->GetValue());

	const int32 MaxMonsters = FMath::RoundToInt32(MaxMonstersSpinBox->GetValue());

	const bool IsLan = LanMatchCheckBox->IsChecked();

	OnSessionCreationConfirmed.Broadcast(SessionName, Username, MaxPlayers, MaxMonsters, IsLan);
}

void USessionCreationWidget::OnBackButtonClickedEvent()
{
	OnSessionCreationCancelled.Broadcast();
}

void USessionCreationWidget::ResetToDefaults() const
{
	SessionNameInput->SetText(FText::FromString("Default Session Name"));

	UsernameInput->SetText(FText::FromString(GetGameInstance<UMyGameInstance>()->GetCustomPlayerData().CustomPlayerName));

	MaxPlayersSpinBox->SetValue(5);

	MaxMonstersSpinBox->SetValue(1);

	LanMatchCheckBox->SetIsChecked(true);
}

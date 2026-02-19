#include "Player/MyPlayerController.h"
#include "Player/MyCharacter.h"
#include "Global/MyBaseLevelGameState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/PauseMenuWidget.h"
#include "UI/WaitingScreenWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Network/OnlineSessionSubsystem.h"
#include "Data/MonsterDataAsset.h"

AMyPlayerController::AMyPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!MappingContext)
	{
		return;
	}

	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		if (TObjectPtr<UEnhancedInputLocalPlayerSubsystem> EILPS = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			EILPS->AddMappingContext(MappingContext, 0);
		}
	}

	MyGI = GetGameInstance<UMyGameInstance>();

	MyGS = Cast<AMyBaseLevelGameState>(UGameplayStatics::GetGameState(this));

	CurrentPlayerID = MyGI->GetCustomPlayerData().CustomPlayerID;

	MyChara = Cast<AMyCharacter>(GetPawn());

	if (MyChara && MyChara->GetCharacterMovement())
	{
		DefaultMaxSpeed = MyChara->GetCharacterMovement()->MaxWalkSpeed;
	}

	PlayerCameraManager->ViewPitchMin = -40;

	PlayerCameraManager->ViewPitchMax = 15;

	SetShowMouseCursor(false);

	SetInputMode(UIOnly);
}

void AMyPlayerController::SetupClient_Implementation(FCustomPlayerData Data, int WaitingTime)
{
	CustomPlayerData = Data;

	SetupInput(CommonInputDataList);

	if (Data.CurrentTeam == ETeam::PLAYER)
	{
		SetupInput(PlayerInputDataList);
	}
	else if (Data.CurrentTeam == ETeam::MONSTER)
	{
		SetupInput(MonsterInputDataList);
	}

	WaitingScreenWidget = CreateWidget<UWaitingScreenWidget>(this, WaitingScreenWidgetClass);

	WaitingScreenWidget->AddToViewport();

	FTimerHandle WaitingScreenHandle;

	// TODO: Je comprends pas, dès fois les clients wait pendant au moins 15 secondes Oo

	GetWorld()->GetTimerManager().SetTimer(WaitingScreenHandle, this, &AMyPlayerController::OnWaitingComplete, WaitingTime, false);
}

void AMyPlayerController::OnWaitingComplete()
{
	RegisterReadyToGameState();
}

void AMyPlayerController::RegisterReadyToGameState_Implementation()
{
	// TODO: Dès fois, j'ai lancé une game sans le waiting screen + countdown
	// Comme si les joueurs étaient ready d'office. A voir si on reproduit

	Cast<AMyBaseLevelGameState>(UGameplayStatics::GetGameState(this))->PlayerHasLoaded();
}

void AMyPlayerController::SetupInput(TArray<FInputData> InputDataList)
{
	if (TObjectPtr<UEnhancedInputComponent> EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		for (FInputData data : InputDataList)
		{
			EIC->BindAction(data.Action, data.Event, this, data.ActionName.GetMemberName());
		}
	}
}

void AMyPlayerController::Move(const FInputActionValue& Value)
{
	FVector Movement = Value.Get<FVector>();

	FRotator CameraRotation = FRotator(0, GetControlRotation().Yaw, 0);

	MyChara->AddMovementInput(CameraRotation.RotateVector(Movement));
}

void AMyPlayerController::Look(const FInputActionValue& Value)
{
	FVector2D Values = Value.Get<FVector2D>();

	AddYawInput(Values.X * CameraSensitivity * GetWorld()->GetDeltaSeconds());

	AddPitchInput(Values.Y * CameraSensitivity * GetWorld()->GetDeltaSeconds());
}

void AMyPlayerController::Jump(const FInputActionValue& Value)
{
	MyChara->Jump();
}

void AMyPlayerController::SprintStart(const FInputActionValue& Value)
{
	MyChara->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed * PlayerSprintFactor;
}

void AMyPlayerController::SprintEnd(const FInputActionValue& Value)
{
	MyChara->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed;
}

void AMyPlayerController::ToggleMenu(const FInputActionValue& Value)
{
	IsPauseMenuOpened = !IsPauseMenuOpened;

	if (IsPauseMenuOpened)
	{
		SetInputMode(UIOnly);
	}
	else
	{
		SetInputMode(GameOnly);
	}

	if (!PauseWidget)
	{
		PauseWidget = CreateWidget<UPauseMenuWidget>(this, PauseWidgetClass);

		PauseWidget->SetVisibility(ESlateVisibility::Hidden);

		PauseWidget->AddToViewport();

		PauseWidget->OnContinueButtonClicked.AddDynamic(this, &AMyPlayerController::OnContinueButtonClicked);

		PauseWidget->OnQuitButtonClicked.AddDynamic(this, &AMyPlayerController::OnQuitButtonClicked);
	}

	PauseWidget->SetVisibility(IsPauseMenuOpened ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	SetShowMouseCursor(IsPauseMenuOpened);
}

void AMyPlayerController::CrouchStart(const FInputActionValue& Value)
{
	MyChara->Crouch();

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Crouch Start");
}

void AMyPlayerController::CrouchEnd(const FInputActionValue& Value)
{
	MyChara->UnCrouch();

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Crouch End");
}

void AMyPlayerController::ToggleFlashlight(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Toggle Flashlight");
}

void AMyPlayerController::Interact(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Interact");
}

void AMyPlayerController::TriggerAttack(const FInputActionValue& Value)
{
	AskToTriggerAttack(CurrentPlayerID);
}

void AMyPlayerController::AskToTriggerAttack_Implementation(FUniqueNetIdRepl PlayerID)
{
	if (CanAttack)
	{
		CanAttack = false;

		PlayAttackAnimation();

		GetWorld()->GetTimerManager().SetTimer(ResetAttackHandle, this, &AMyPlayerController::ResetAttack, MyGS->RetrieveMonsterData(MyGI->RetrieveServerPlayerData(PlayerID).MonsterType)->MonsterAttackCooldown, false);
	}
}

void AMyPlayerController::PlayAttackAnimation_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Player Attack");
}

void AMyPlayerController::ResetAttack()
{
	CanAttack = true;
}

void AMyPlayerController::TriggerSpecial(const FInputActionValue& Value)
{
	AskToTriggerSpecial(CurrentPlayerID);
}

void AMyPlayerController::AskToTriggerSpecial_Implementation(FUniqueNetIdRepl PlayerID)
{
	if (CanTriggerSpecial)
	{
		CanTriggerSpecial = false;

		PlaySpecialAnimation();

		GetWorld()->GetTimerManager().SetTimer(ResetSpecialHandle, this, &AMyPlayerController::ResetSpecial, MyGS->RetrieveMonsterData(MyGI->RetrieveServerPlayerData(PlayerID).MonsterType)->MonsterSpecialCooldown, false);
	}
}

void AMyPlayerController::PlaySpecialAnimation_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Player Special");
}

void AMyPlayerController::ResetSpecial()
{
	CanTriggerSpecial = true;
}

void AMyPlayerController::OnContinueButtonClicked()
{
	FInputActionValue _;

	ToggleMenu(_);
}

void AMyPlayerController::OnQuitButtonClicked()
{
	if (HasAuthority())
	{
		Cast<AMyBaseLevelGameState>(UGameplayStatics::GetGameState(this))->DestroyGame();
	}
	else
	{
		DestroySessionOnClient();
	}
}

void AMyPlayerController::DestroySessionOnClient_Implementation()
{
	GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>()->DestroySession();

	ClientTravel("/Game/Levels/MainMenu", ETravelType::TRAVEL_Absolute);
}

void AMyPlayerController::ToggleWaitingScreenOff_Implementation()
{
	WaitingScreenWidget->PlayWaitingScreenFadeOutAnimation();
}

void AMyPlayerController::DisplayCountdown_Implementation(int Countdown)
{
	WaitingScreenWidget->SetCountdownText(FString::FromInt(Countdown));

	if (Countdown <= 0)
	{
		SetInputMode(GameOnly);

		if (CustomPlayerData.CurrentTeam == ETeam::PLAYER)
		{
			WaitingScreenWidget->SetCountdownText("SURVIVE!");
		}
		else if (CustomPlayerData.CurrentTeam == ETeam::MONSTER)
		{
			WaitingScreenWidget->SetCountdownText("HUNT!");
		}

		WaitingScreenWidget->PlayCountdownTextPopupAnimation();
	}
}

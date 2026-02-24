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
#include "Player/Capacity/BaseAbilityComponent.h"

AMyPlayerController::AMyPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
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

		MyChara = Cast<AMyCharacter>(GetPawn());

		PlayerCameraManager->ViewPitchMin = -40;

		PlayerCameraManager->ViewPitchMax = 15;

		SetShowMouseCursor(false);

		SetInputMode(UIOnly);
	}
}

void AMyPlayerController::SetupServer_Implementation(FCustomPlayerData Data, class UMonsterDataAsset* MonsterData)
{
	CustomPlayerData = Data;

	if (Data.CurrentTeam == ETeam::MONSTER)
	{
		MyMonsterData = MonsterData;
	}
}

void AMyPlayerController::SetupClient_Implementation(FCustomPlayerData Data, UMonsterDataAsset* MonsterData, int WaitingTime)
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

		MyMonsterData = MonsterData;
	}

	WaitingScreenWidget = CreateWidget<UWaitingScreenWidget>(this, WaitingScreenWidgetClass);

	WaitingScreenWidget->AddToViewport();

	FTimerHandle WaitingScreenHandle;

	GetWorld()->GetTimerManager().SetTimer(WaitingScreenHandle, this, &AMyPlayerController::OnWaitingComplete, WaitingTime, false);
}

void AMyPlayerController::OnWaitingComplete()
{
	RegisterReadyToGameState();
}

void AMyPlayerController::RegisterReadyToGameState_Implementation()
{
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
	if (IsInteracting)
	{
		return;
	}

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
	if (IsInteracting)
	{
		return;
	}

	MyChara->Jump();
}

void AMyPlayerController::SprintStart(const FInputActionValue& Value)
{
	if (!HasAuthority())
	{
		DefaultMaxSpeed = MyChara->GetCharacterMovement()->MaxWalkSpeed;

		MyChara->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed * PlayerSprintFactor;
	}

	SetIsSprintingOnServer(true);
}

void AMyPlayerController::SprintEnd(const FInputActionValue& Value)
{
	if (!HasAuthority())
	{
		MyChara->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed;
	}

	SetIsSprintingOnServer(false);
}

void AMyPlayerController::SetIsSprintingOnServer_Implementation(bool IsSprinting)
{
	if (IsSprinting)
	{
		DefaultMaxSpeed = Cast<AMyCharacter>(GetPawn())->GetCharacterMovement()->MaxWalkSpeed;

		Cast<AMyCharacter>(GetPawn())->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed * PlayerSprintFactor;
	}
	else
	{
		Cast<AMyCharacter>(GetPawn())->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed;
	}
}

void AMyPlayerController::ToggleMenu(const FInputActionValue& Value)
{
	IsPauseMenuOpened = !IsPauseMenuOpened;

	InteractStop(Value);

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
	MyChara->ToggleFlashlight();
}

void AMyPlayerController::InteractStart(const FInputActionValue& Value)
{
	IsInteracting = true;

	MyChara->InteractWithSurroundingActor();
}

void AMyPlayerController::InteractStop(const FInputActionValue& Value)
{
	IsInteracting = false;

	MyChara->StopInteractingWithActor();
}

void AMyPlayerController::TriggerAttack(const FInputActionValue& Value)
{
	AskToTriggerAttack();
}

void AMyPlayerController::AskToTriggerAttack_Implementation()
{
	if (CanAttack && MyMonsterData)
	{
		CanAttack = false;

		PlayAttackAnimation();

		GetWorld()->GetTimerManager().SetTimer(ResetAttackHandle, this, &AMyPlayerController::ResetAttack, MyMonsterData->MonsterAttackCooldown, false);
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
	AskToTriggerSpecial();
}

void AMyPlayerController::AskToTriggerSpecial_Implementation()
{
	if (CanTriggerSpecial && MyMonsterData)
	{
		CanTriggerSpecial = false;

		GetPawn()->FindComponentByClass<UBaseAbilityComponent>()->StartAbility(MyMonsterData);

		GetWorld()->GetTimerManager().SetTimer(ResetSpecialHandle, this, &AMyPlayerController::ResetSpecial, MyMonsterData->MonsterSpecialCooldown, false);
	}
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

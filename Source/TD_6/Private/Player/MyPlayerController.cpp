#include "Player/MyPlayerController.h"
#include "Player/MyCharacter.h"
#include "Global/MyBaseLevelGameState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/PauseMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Network/OnlineSessionSubsystem.h"

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

	MyChara = Cast<AMyCharacter>(GetPawn());

	if (MyChara && MyChara->GetCharacterMovement())
	{
		DefaultMaxSpeed = MyChara->GetCharacterMovement()->MaxWalkSpeed;
	}

	PlayerCameraManager->ViewPitchMin = -40;

	PlayerCameraManager->ViewPitchMax = 15;

	SetShowMouseCursor(false);

	SetInputMode(GameOnly);
}

void AMyPlayerController::SetupClient_Implementation(FCustomPlayerData Data)
{
	SetupCommonInput();

	if (Data.CurrentTeam == ETeam::PLAYER)
	{
		SetupPlayerInput();
	}
	else if (Data.CurrentTeam == ETeam::MONSTER)
	{
		SetupMonsterInput();
	}
}

void AMyPlayerController::SetupCommonInput()
{
	if (TObjectPtr<UEnhancedInputComponent> EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		for (FInputData data : CommonInputDataList)
		{
			EIC->BindAction(data.Action, data.Event, this, data.ActionName.GetMemberName());
		}
	}
}

void AMyPlayerController::SetupPlayerInput()
{
	if (TObjectPtr<UEnhancedInputComponent> EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		for (FInputData data : PlayerInputDataList)
		{
			EIC->BindAction(data.Action, data.Event, this, data.ActionName.GetMemberName());
		}
	}
}

void AMyPlayerController::SetupMonsterInput()
{
	if (TObjectPtr<UEnhancedInputComponent> EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		for (FInputData data : MonsterInputDataList)
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
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Trigger Attack");
}

void AMyPlayerController::TriggerSpecial(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Trigger Special");
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

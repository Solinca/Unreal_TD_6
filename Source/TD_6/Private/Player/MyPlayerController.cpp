#include "Player/MyPlayerController.h"
#include "Player/MyCharacter.h"
#include "Global/MyBaseLevelGameState.h"
#include "Global/BaseLevelWorldSettings.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/PauseMenuWidget.h"
#include "UI/WaitingScreenWidget.h"
#include "UI/GlobalTimerWidget.h"
#include "UI/HudMonsterWidget.h"
#include "UI/HudSurvivorWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Network/OnlineSessionSubsystem.h"
#include "Data/MonsterDataAsset.h"
#include "Player/Capacity/BaseAbilityComponent.h"
#include "Engine/Light.h"
#include "Components/AudioComponent.h"

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

		HudSurvivorWidget = CreateWidget<UHudSurvivorWidget>(this, HudSurvivorClass);

		HudSurvivorWidget->AddToViewport();
	}
	else if (Data.CurrentTeam == ETeam::MONSTER)
	{
		SetupInput(MonsterInputDataList);

		MyMonsterData = MonsterData;

		HudMonsterWidget = CreateWidget<UHudMonsterWidget>(this, HudMonsterClass);

		HudMonsterWidget->AddToViewport();
	}

	WaitingScreenWidget = CreateWidget<UWaitingScreenWidget>(this, WaitingScreenWidgetClass);

	WaitingScreenWidget->AddToViewport();

	GlobalTimerWidget = CreateWidget<UGlobalTimerWidget>(this, GlobalTimerWidgetClass);

	GlobalTimerWidget->AddToViewport();

	GlobalTimerWidget->SetVisibility(ESlateVisibility::Hidden);

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
	if (IsInteracting || MyChara->IsPlayerDead())
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
	if (IsInteracting || MyChara->IsPlayerDead())
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
	UCharacterMovementComponent* CMC = Cast<AMyCharacter>(GetPawn())->GetCharacterMovement();

	if (IsSprinting)
	{
		DefaultMaxSpeed = CMC->MaxWalkSpeed;

		CMC->MaxWalkSpeed = DefaultMaxSpeed * PlayerSprintFactor;
	}
	else
	{
		CMC->MaxWalkSpeed = DefaultMaxSpeed;
	}
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

	FlushPressedKeys();

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
	if (MyChara->IsPlayerDead())
	{
		return;
	}

	MyChara->Crouch();
}

void AMyPlayerController::CrouchEnd(const FInputActionValue& Value)
{
	if (MyChara->IsPlayerDead())
	{
		return;
	}

	MyChara->UnCrouch();
}

void AMyPlayerController::ToggleFlashlight(const FInputActionValue& Value)
{
	if (MyChara->IsPlayerDead())
	{
		return;
	}

	MyChara->ToggleFlashlight();

	HudSurvivorWidget->ToggleFlashlight();
}

void AMyPlayerController::InteractStart(const FInputActionValue& Value)
{
	if (MyChara->IsPlayerDead())
	{
		return;
	}

	IsInteracting = true;

	MyChara->InteractWithSurroundingActor();
}

void AMyPlayerController::InteractStop(const FInputActionValue& Value)
{
	if (MyChara->IsPlayerDead())
	{
		return;
	}

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

		Cast<AMyCharacter>(GetPawn())->SetIsAttacking();

		GetWorld()->GetTimerManager().SetTimer(ResetAttackHandle, this, &AMyPlayerController::ResetAttack, MyMonsterData->MonsterAttackCooldown, false);
	}
}

void AMyPlayerController::ResetAttack()
{
	CanAttack = true;
}

void AMyPlayerController::TriggerSpecial(const FInputActionValue& Value)
{
	AskToTriggerSpecial();
}

void AMyPlayerController::Client_StartSkillCooldown_Implementation(float Cooldown)
{
	if (HudMonsterWidget)
	{
		HudMonsterWidget->StartSkillCooldown(Cooldown);
	}
}

void AMyPlayerController::AskToTriggerSpecial_Implementation()
{
	if (CanTriggerSpecial && MyMonsterData)
	{
		CanTriggerSpecial = false;

		GetPawn()->FindComponentByClass<UBaseAbilityComponent>()->StartAbility(MyMonsterData);

		GetWorld()->GetTimerManager().SetTimer(
			ResetSpecialHandle,
			this,
			&AMyPlayerController::ResetSpecial,
			MyMonsterData->MonsterSpecialCooldown,
			false);

		Client_StartSkillCooldown(MyMonsterData->MonsterSpecialCooldown);
	}
}

void AMyPlayerController::ResetSpecial()
{
	CanTriggerSpecial = true;

	GetPawn()->FindComponentByClass<UBaseAbilityComponent>()->StopAbility();
}

void AMyPlayerController::TriggerScream(const FInputActionValue& Value)
{
	AskToTriggerScream();
}

void AMyPlayerController::AskToTriggerScream_Implementation()
{
	if (!CanTriggerScream || !MyMonsterData)
	{
		return;
	}

	if (!MyMonsterData->ScreamSound)
	{
		return;
	}

	CanTriggerScream = false;

	const float Roll = FMath::FRand();
	const bool bIsMonsterCry = Roll < MyMonsterData->MonsterCryChance;

	USoundBase* SoundToPlay = bIsMonsterCry && MyMonsterData->MonsterRevealCrySound
		? MyMonsterData->MonsterRevealCrySound
		: MyMonsterData->ScreamSound;

	if (UBaseAbilityComponent* AbilityComp = GetPawn()->FindComponentByClass<UBaseAbilityComponent>())
	{
		AbilityComp->MulticastPlayScreamWithSound(SoundToPlay);
	}

	GetWorld()->GetTimerManager().SetTimer(
		ResetScreamHandle,
		this,
		&AMyPlayerController::ResetScream,
		MyMonsterData->ScreamCooldown,
		false
	);

	Client_StartScreamCooldown(MyMonsterData->ScreamCooldown);
}

void AMyPlayerController::ResetScream()
{
	CanTriggerScream = true;
}

void AMyPlayerController::Client_StartScreamCooldown_Implementation(float Cooldown)
{
	if (HudMonsterWidget)
	{
		HudMonsterWidget->StartScreamCooldown(Cooldown);
	}
}

void AMyPlayerController::OnContinueButtonClicked()
{
	ToggleMenu(FInputActionValue{});
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

void AMyPlayerController::KillPlayer_Implementation()
{
	Cast<AMyCharacter>(GetPawn())->KillPlayer();
}

void AMyPlayerController::ResurrectPlayer_Implementation()
{
	Cast<AMyCharacter>(GetPawn())->ResurrectPlayer();
}

void AMyPlayerController::DestroySessionOnClient_Implementation()
{
	GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>()->DestroySession();

	ClientTravel("/Game/Levels/MainMenu", ETravelType::TRAVEL_Absolute);
}

void AMyPlayerController::ToggleWaitingScreenOff_Implementation()
{
	WaitingScreenWidget->PlayWaitingScreenFadeOutAnimation();

	BaseAmbianceAudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), BaseAmbianceMusic);
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

		GlobalTimerWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMyPlayerController::DisplayGlobalTimer_Implementation(int GlobalTimer)
{
	GlobalTimerWidget->SetGlobalTimer(GlobalTimer);
}

void AMyPlayerController::SetupResultScreenServer_Implementation(FVector TargetPosition, FRotator TargetRotation)
{
	ResultScreenTargetPosition = TargetPosition;

	ResultScreenTargetRotation = TargetRotation;

	Cast<AMyCharacter>(GetPawn())->PreparePlayerForEndScreen();
}

void AMyPlayerController::SetupResultScreenClient_Implementation(ETeam WinningTeam)
{
	if (IsPauseMenuOpened)
	{
		ToggleMenu(FInputActionValue{});
	}

	SetInputMode(UIOnly);

	FlushPressedKeys();

	MyChara->GetCharacterMovement()->bOrientRotationToMovement = false;

	GlobalTimerWidget->SetVisibility(ESlateVisibility::Hidden);

	if (HudSurvivorWidget)
	{
		HudSurvivorWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	if (HudMonsterWidget)
	{
		HudMonsterWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	IsWinning = CustomPlayerData.CurrentTeam == WinningTeam;

	ABaseLevelWorldSettings* BLWS = Cast<ABaseLevelWorldSettings>(GetWorld()->GetWorldSettings());

	BLWS->EndScreenLighting->SetLightColor(WinningTeam == ETeam::MONSTER ? BLWS->MonsterWinningColor : BLWS->PlayerWinningColor);

	PlayerCameraManager->StartCameraFade(0, 1, 1, FColor::Black, false, true);

	BaseAmbianceAudioComponent->FadeOut(1, 0);

	GetWorld()->GetTimerManager().SetTimer(ResultScreenTransitionHandle, this, &AMyPlayerController::DisplayResultScreen, 1, false);
}

void AMyPlayerController::DisplayResultScreen()
{
	ABaseLevelWorldSettings* BLWS = Cast<ABaseLevelWorldSettings>(GetWorld()->GetWorldSettings());

	SetViewTarget(BLWS->EndScreenCamera);

	PlayerCameraManager->StartCameraFade(1, 0, 0.2f, FColor::Black, false, true);

	AskToTeleportPlayerToResultScreen();

	UGameplayStatics::PlaySound2D(GetWorld(), IsWinning ? VictoryMusic : DefeatMusic);
}

void AMyPlayerController::AskToTeleportPlayerToResultScreen_Implementation()
{
	Cast<AMyCharacter>(GetPawn())->SetActorLocationAndRotation(ResultScreenTargetPosition, ResultScreenTargetRotation, false, nullptr, ETeleportType::TeleportPhysics);

	SetControlRotation(ResultScreenTargetRotation);

	ForceClientRotation(ResultScreenTargetPosition, ResultScreenTargetRotation);
}

void AMyPlayerController::ForceClientRotation_Implementation(FVector TargetPosition, FRotator TargetRotation)
{
	MyChara->SetActorLocationAndRotation(TargetPosition, TargetRotation, false, nullptr, ETeleportType::TeleportPhysics);

	SetControlRotation(TargetRotation);
}

#include "Player/MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SpotLightComponent.h"
#include "Net/UnrealNetwork.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");

	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");

	Camera->SetupAttachment(SpringArm);

	Flashlight = CreateDefaultSubobject<USpotLightComponent>("Flashlight");

	Flashlight->SetupAttachment(RootComponent);
}

void AMyCharacter::ToggleFlashlight_Implementation()
{
	IsFlashlightOn = !IsFlashlightOn;

	SetFlashlightVisibility();
}

void AMyCharacter::SetFlashlightVisibility()
{
	Flashlight->SetVisibility(IsFlashlightOn, true);
}

void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyCharacter, IsFlashlightOn);
}

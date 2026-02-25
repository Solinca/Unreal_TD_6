#include "Player/Capacity/TrapActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/MyCharacter.h"
#include "Global/MyGameInstance.h"

ATrapActor::ATrapActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetSphereRadius(TriggerRadius);
	TriggerSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	TriggerSphere->SetGenerateOverlapEvents(true);
	SetRootComponent(TriggerSphere);

	TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMesh"));
	TrapMesh->SetupAttachment(TriggerSphere);
	TrapMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATrapActor::InitTrap(float InSnareTime)
{
	SnareTime = InSnareTime;

	if (HasAuthority())
	{
		TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ATrapActor::OnTriggerOverlap);
	}
}

void ATrapActor::OnTriggerOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority() || bIsTriggered)
	{
		return;
	}

	AMyCharacter* OtherCharacter = Cast<AMyCharacter>(OtherActor);
	if (!OtherCharacter)
	{
		return;
	}

	if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
	{
		if (const APlayerController* PC = Cast<APlayerController>(OtherCharacter->GetController()))
		{
			if (PC->GetNetConnection())
			{
				const FCustomPlayerData PlayerData = GI->RetrieveServerPlayerData(PC->GetPlayerState<APlayerState>()->GetUniqueId());
				if (PlayerData.CurrentTeam != ETeam::PLAYER)
				{
					return;
				}
			}
		}
	}

	bIsTriggered = true;
	SnarePlayer(OtherCharacter);
}

void ATrapActor::SnarePlayer(AMyCharacter* TargetCharacter)
{
	if (!TargetCharacter || !TargetCharacter->GetCharacterMovement())
	{
		return;
	}

	SnaredCharacter = TargetCharacter;

	TargetCharacter->GetCharacterMovement()->DisableMovement();

	MulticastOnTrapTriggered(TargetCharacter);

	GetWorld()->GetTimerManager().SetTimer(
		SnareTimerHandle,
		this,
		&ATrapActor::ReleasePlayer,
		SnareTime,
		false
	);
}

void ATrapActor::MulticastOnTrapTriggered_Implementation(AMyCharacter* TargetCharacter)
{
	if (TrapTriggeredSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, TrapTriggeredSound, GetActorLocation());
	}

	if (TrapMesh)
	{
		TrapMesh->SetVisibility(false);
	}
}

void ATrapActor::ReleasePlayer()
{
	if (SnaredCharacter.IsValid() && SnaredCharacter->GetCharacterMovement())
	{
		SnaredCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	MulticastOnPlayerReleased();

	Destroy();
}

void ATrapActor::MulticastOnPlayerReleased_Implementation()
{
	// VFX / SFX player is free
}

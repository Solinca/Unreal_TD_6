#include "Player/Capacity/TrapActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
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

	MulticastOnTrapPlaced();

	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ATrapActor::OnTriggerOverlap);
}

void ATrapActor::MulticastOnTrapPlaced_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, TrapPlacedSound, GetActorLocation());
}

void ATrapActor::OnTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* OtherCharacter = Cast<AMyCharacter>(OtherActor);

	if (bIsTriggered || !OtherCharacter || OtherActor->Tags.Contains("MONSTER"))
	{
		return;
	}

	bIsTriggered = true;

	SnarePlayer(OtherCharacter);
}

void ATrapActor::SnarePlayer(AMyCharacter* TargetCharacter)
{
	SnaredCharacter = TargetCharacter;

	TargetCharacter->SnarePlayerServerSide();

	MulticastOnTrapTriggered(TargetCharacter);

	GetWorld()->GetTimerManager().SetTimer(SnareTimerHandle, this, &ATrapActor::ReleasePlayer, SnareTime, false);
}

void ATrapActor::MulticastOnTrapTriggered_Implementation(AMyCharacter* TargetCharacter)
{
	UGameplayStatics::PlaySoundAtLocation(this, TrapTriggeredSound, GetActorLocation());

	TrapMesh->SetVisibility(false);
}

void ATrapActor::ReleasePlayer()
{
	if (SnaredCharacter.IsValid())
	{
		SnaredCharacter->ReleasePlayerServerSide();
	}

	Destroy();
}

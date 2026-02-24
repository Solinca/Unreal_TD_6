#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HookProjectile.generated.h"

class USphereComponent;
class UHookAbilityComponent;
class AMyCharacter;

UENUM()
enum class EHookState : uint8
{
	Traveling,
	Pulling,
	Returning,
	Finished
};

UCLASS()
class TD_6_API AHookProjectile : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Hook")
	TObjectPtr<USphereComponent> CollisionSphere = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Hook")
	TObjectPtr<UStaticMeshComponent> HookMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Pull")
	float PullDuration = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Pull")
	float PullArrivalDistance = 120.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Pull")
	float PullTimeout = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Settings")
	float CollisionRadius = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Settings")
	float ReturnArrivalDistance = 100.f;

	TWeakObjectPtr<UHookAbilityComponent> OwningAbility = nullptr;
	TWeakObjectPtr<AMyCharacter> CachedOwnerCharacter = nullptr;
	TWeakObjectPtr<AMyCharacter> HookedCharacter = nullptr;

	FVector LaunchOrigin = FVector::ZeroVector;
	FVector TravelDirection = FVector::ZeroVector;

	float MaxDistance = 0.f;
	float HookSpeed = 0.f;
	float ReelingTime = 0.f;
	float ReturnSpeed = 0.f;
	float PullSpeed = 0.f;
	float PullElapsedTime = 0.f;

	EHookState CurrentState = EHookState::Finished;

	UFUNCTION()
	void OnHitSomething(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void StartPulling(AMyCharacter* TargetCharacter);
	void ReleasePulledPlayer();
	void StartReturning();
	void FinishHook();

	void FreezeCharacter(AMyCharacter* Character);
	void UnfreezeCharacter(AMyCharacter* Character);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHookHit();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHookMiss();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHookFinished();

	bool IsPlayerTeam(AMyCharacter* Character) const;

public:
	AHookProjectile();

	virtual void Tick(float DeltaTime) override;

	void Traveling(float DeltaTime);
	void Pulling(float DeltaTime);
	void Returning(float DeltaTime);

	void InitHook(UHookAbilityComponent* InAbility, float InMaxDistance, float InHookSpeed, float InReelingTime);

	void ForceCleanup();
};
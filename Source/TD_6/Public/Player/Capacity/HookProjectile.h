#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HookProjectile.generated.h"

UENUM(BlueprintType)
enum class EHookState : uint8
{
	Idle,
	Traveling,
	Pulling,
	Returning,
	Finished
};

UCLASS()
class AHookProjectile : public AActor
{
	GENERATED_BODY()

private:
	TObjectPtr<UAudioComponent> HookReelingSound;

	TWeakObjectPtr<class UHookAbilityComponent> OwningAbility;

	TWeakObjectPtr<class AMyCharacter> CachedOwnerCharacter;

	TWeakObjectPtr<class AMyCharacter> HookedCharacter;

	float MaxDistance = 0.f;

	float HookSpeed = 0.f;

	float ReelingTime = 0.f;

	float PullSpeed = 0.f;

	float ReturnSpeed = 0.f;

	float PullElapsedTime = 0.f;

	void Traveling(float DeltaTime);

	void Pulling(float DeltaTime);

	void Returning(float DeltaTime);

	void StartPulling(class AMyCharacter* TargetCharacter);

	void StartReturning();

	void ReleasePulledPlayer();

	void FinishHook();

	void UpdateHookScale();

	FVector GetHookTipLocation() const;

	UFUNCTION()
	void OnRep_CurrentReach();

	UFUNCTION()
	void OnRep_CurrentState();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHookFinished();

protected:
	AHookProjectile();

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> HookMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Config")
	float CollisionRadius = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Config")
	float BaseHookLength = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Pulling")
	float PullDuration = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Pulling")
	float PullTimeout = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Pulling")
	float PullArrivalDistance = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Returning")
	float ReturnArrivalDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundBase* AbilityTriggerSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundBase* AbilityOnGoingSound = nullptr;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentState)
	EHookState CurrentState = EHookState::Idle;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentReach)
	float CurrentReach = 0.f;

	UPROPERTY(Replicated)
	FVector LaunchOrigin = FVector::ZeroVector;

	UPROPERTY(Replicated)
	FVector TravelDirection = FVector::ForwardVector;

public:
	void InitHook(class UHookAbilityComponent* InAbility, float InMaxDistance, float InHookSpeed, float InReelingTime);

	void ForceCleanup();
};

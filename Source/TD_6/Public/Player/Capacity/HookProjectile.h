#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HookProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class AMyCharacter;
class UHookAbilityComponent;

UENUM(BlueprintType)
enum class EHookState : uint8
{
	Idle,
	Traveling,
	Returning,
	Finished
};

UCLASS()
class AHookProjectile : public AActor
{
	GENERATED_BODY()

public:
	AHookProjectile();

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitHook(UHookAbilityComponent* InAbility, const FVector& InLaunchDirection, float InMaxDistance, float InHookSpeed, float InReelingTime);
	void ForceCleanup();
	FVector GetHookTipLocation() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> HookMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> BeamNiagara;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|VFX")
	TObjectPtr<UNiagaraSystem> BeamNiagaraSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|VFX")
	FName BeamStartParamName = TEXT("BeamStart");

	UPROPERTY(EditDefaultsOnly, Category = "Hook|VFX")
	FName BeamEndParamName = TEXT("BeamEnd");

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Config")
	float CollisionRadius = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Config")
	float SpinSpeed = 1080.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Config")
	FRotator MeshRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Config")
	float HookGravity = 490.f;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentState)
	EHookState CurrentState = EHookState::Idle;

	UPROPERTY(Replicated)
	FVector LaunchOrigin = FVector::ZeroVector;

	UPROPERTY(Replicated)
	FVector LaunchVelocity = FVector::ZeroVector;

	UPROPERTY(Replicated)
	float FlightTime = 0.f;

	UPROPERTY(Replicated)
	FVector ReturnStartLocation = FVector::ZeroVector;

	UPROPERTY(Replicated)
	float ReturnAlpha = 1.f;

	UPROPERTY(Replicated)
	float ReelingTime = 0.f;

	float LocalFlightTime = 0.f;
	float LocalReturnAlpha = 1.f;

	float MaxDistance = 0.f;
	float HookSpeed = 0.f;
	float CurrentSpinAngle = 0.f;

	TWeakObjectPtr<UHookAbilityComponent> OwningAbility;
	TWeakObjectPtr<AMyCharacter> CachedOwnerCharacter;

private:

	void Traveling(float DeltaTime);
	void Returning(float DeltaTime);
	void ClientSimulate(float DeltaTime);
	void StartReturning();
	void FinishHook();

	FVector ComputeBallisticPosition(float T) const;
	FVector ComputeBallisticVelocity(float T) const;

	void UpdateHookPosition();
	void UpdateBeam();
	void EnsureBeamActive();

	UFUNCTION()
	void OnRep_CurrentState();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastOnHookHit();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastOnHookMiss();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastOnHookFinished();
};
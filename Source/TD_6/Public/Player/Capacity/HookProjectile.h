#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HookProjectile.generated.h"

UENUM(BlueprintType)
enum class EHookState : uint8
{
	Traveling,
	Returning,
	Finished
};

UCLASS()
class AHookProjectile : public AActor
{
	GENERATED_BODY()

private:
	EHookState CurrentState = EHookState::Traveling;

	FVector LaunchOrigin = FVector::ZeroVector;

	FVector LaunchVelocity = FVector::ZeroVector;

	FVector ReturnStartLocation = FVector::ZeroVector;

	FHitResult Hit;

	float MaxDistance = 0;

	float FlightTime = 0;

	float ReelingTime = 0;

	float ReturnAlpha = 1;

	FVector ComputeBallisticPosition(float DeltaTime);

	FVector ComputeBallisticVelocity(float DeltaTime);

	FVector GetHookTipLocation();

	void Traveling(float DeltaTime);

	void Returning(float DeltaTime);

	void StartReturning();

	void FinishHook();

	void UpdateHookPosition(float DeltaTime);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartAbility();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateAbility(FVector TargetPosition);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHookHit();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHookMiss();

protected:
	AHookProjectile();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> HookMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UNiagaraComponent> BeamNiagara;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|VFX")
	TObjectPtr<class UNiagaraSystem> BeamNiagaraSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|VFX")
	FName BeamStartParamName = TEXT("BeamStart");

	UPROPERTY(EditDefaultsOnly, Category = "Hook|VFX")
	FName BeamEndParamName = TEXT("BeamEnd");

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Config")
	float CollisionRadius = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hook|Config")
	float HookGravity = 490.f;

public:
	void InitHook(const FVector& InLaunchDirection, float InMaxDistance, float InHookSpeed, float InReelingTime);
};

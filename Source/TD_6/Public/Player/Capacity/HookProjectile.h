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
	UPROPERTY(Transient)
	TArray<AActor*> ActorsToIgnore;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	EHookState CurrentState = EHookState::Traveling;

	FVector LaunchOrigin = FVector::ZeroVector;

	FVector LaunchVelocity = FVector::ZeroVector;

	FVector ReturnStartLocation = FVector::ZeroVector;

	float FlightTime = 0;

	float ReelingTime = 0;

	float ReturnAlpha = 1;

	FVector GetHookTipLocation();

	void Traveling(float DeltaTime);

	void Returning(float DeltaTime);

	void StartReturning();

	void FinishHook();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartAbility();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateAbility(const FVector& TargetPosition, const float DeltaTime);

protected:
	AHookProjectile();

	virtual void Tick(float DeltaTime) override;

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
	float CollisionRadius = 10.f;

public:
	void InitHook(const FVector& InLaunchDirection, float InHookSpeed, float InReelingTime);
};

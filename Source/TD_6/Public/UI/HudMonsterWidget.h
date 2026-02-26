#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HudMonsterWidget.generated.h"

class URadialProgressBarImage;

UCLASS()
class TD_6_API UHudMonsterWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "MonsterHUD|Skill")
	void StartSkillCooldown(float Duration);

	UFUNCTION(BlueprintCallable, Category = "MonsterHUD|Skill")
	void CancelSkillCooldown();

	UFUNCTION(BlueprintPure, Category = "MonsterHUD|Skill")
	bool IsSkillReady() const;
	
	UFUNCTION(BlueprintCallable, Category = "MonsterHUD|Scream")
	void StartScreamCooldown(float Duration);

	UFUNCTION(BlueprintCallable, Category = "MonsterHUD|Scream")
	void CancelScreamCooldown();

	UFUNCTION(BlueprintPure, Category = "MonsterHUD|Scream")
	bool IsScreamReady() const;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<URadialProgressBarImage> SkillRadialBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<URadialProgressBarImage> ScreamRadialBar;
};
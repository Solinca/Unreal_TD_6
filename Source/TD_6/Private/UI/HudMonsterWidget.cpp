#include "UI/HudMonsterWidget.h"
#include "UI/RadialProgressBarImage.h"

void UHudMonsterWidget::StartSkillCooldown(float Duration)
{
	if (SkillRadialBar)
	{
		SkillRadialBar->StartCooldown(Duration);
	}
}

void UHudMonsterWidget::CancelSkillCooldown()
{
	if (SkillRadialBar)
	{
		SkillRadialBar->CancelCooldown();
	}
}

bool UHudMonsterWidget::IsSkillReady() const
{
	return SkillRadialBar ? SkillRadialBar->IsReady() : true;
}

void UHudMonsterWidget::StartScreamCooldown(float Duration)
{
	if (ScreamRadialBar)
	{
		ScreamRadialBar->StartCooldown(Duration);
	}
}

void UHudMonsterWidget::CancelScreamCooldown()
{
	if (ScreamRadialBar)
	{
		ScreamRadialBar->CancelCooldown();
	}
}

bool UHudMonsterWidget::IsScreamReady() const
{
	return ScreamRadialBar ? ScreamRadialBar->IsReady() : true;
}
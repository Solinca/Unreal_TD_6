---
name: ue-widget
description: Create a new UMG Widget class with bound widgets and delegate patterns
user-invocable: true
---

# Create UE Widget

Generate a new UMG Widget C++ class following this project's UI conventions.

## Instructions

1. Ask the user for:
   - **Widget name** (e.g., `SettingsMenuWidget`, `InventorySlotWidget`)
   - **Bound widgets** — list of UI elements to bind (Button, TextBlock, VerticalBox, HorizontalBox, ScrollBox, Image, etc.)
   - **Delegates** — any button click or custom delegates to expose

2. Generate the **header** at `Source/TD_6/Public/UI/<WidgetName>.h`:

   ```cpp
   #pragma once

   #include "CoreMinimal.h"
   #include "Blueprint/UserWidget.h"
   #include "<WidgetName>.generated.h"

   // Delegate declarations
   DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOn<Action>Signature);

   UCLASS()
   class TD_6_API U<WidgetName> : public UUserWidget
   {
       GENERATED_BODY()

   private:
       // Button click handlers
       UFUNCTION()
       void On<ButtonName>ClickedEvent();

   protected:
       virtual void NativeConstruct() override;

       // Bound widgets use TObjectPtr with forward declaration
       UPROPERTY(meta = (BindWidget))
       TObjectPtr<class UButton> <ButtonName> = nullptr;

       UPROPERTY(meta = (BindWidget))
       TObjectPtr<class UTextBlock> <TextName> = nullptr;

   public:
       // Public delegates
       FOn<Action>Signature On<Action>;
   };
   ```

3. Generate the **source** at `Source/TD_6/Private/UI/<WidgetName>.cpp`:
   - First include is the widget's own header: `#include "UI/<WidgetName>.h"`
   - Include UMG component headers (`Components/Button.h`, `Components/TextBlock.h`, etc.)
   - `NativeConstruct()` calls `Super::NativeConstruct()` then binds button delegates with `AddDynamic`
   - Each button handler broadcasts its delegate

4. **Conventions** from this project:
   - Use `TObjectPtr<class UType>` with forward declarations (not raw pointers)
   - Initialize bound widget pointers to `nullptr`
   - Use `meta = (BindWidget)` for widgets that must exist in the Blueprint
   - Button click handlers are private `UFUNCTION()` methods named `On<Name>ClickedEvent`
   - Public delegates use `DECLARE_DYNAMIC_MULTICAST_DELEGATE` with `Signature` suffix
   - Sub-widget classes referenced via `TSubclassOf<UUserWidget>` with `EditDefaultsOnly, BlueprintReadOnly, Category = "UI"`

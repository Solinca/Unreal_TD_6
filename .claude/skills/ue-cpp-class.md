---
name: ue-cpp-class
description: Create a new Unreal Engine C++ class with proper boilerplate
user-invocable: true
---

# Create UE C++ Class

Generate a new C++ header and source file pair following this project's conventions.

## Instructions

1. Ask the user for:
   - **Class name** (without prefix — you add the appropriate `A`, `U`, or `F` prefix)
   - **Parent class** (e.g., AActor, UActorComponent, UUserWidget, UGameInstanceSubsystem, ACharacter, APlayerController, AGameModeBase, AGameStateBase, APlayerState)
   - **System folder** — which subfolder under `Source/TD_6/Public/` and `Source/TD_6/Private/` (e.g., `Global`, `Network`, `Player`, `UI`)

2. Generate the **header** at `Source/TD_6/Public/<SystemFolder>/<ClassName>.h` following these rules:
   - `#pragma once`
   - `#include "CoreMinimal.h"`
   - Include the parent class header
   - `#include "<ClassName>.generated.h"` as the **last include**
   - `UCLASS()` macro
   - Class declared as `class TD_6_API <ClassName> : public <ParentClass>`
   - `GENERATED_BODY()` as first line in class body
   - Sections ordered: `private:`, `protected:`, `public:`

3. Generate the **source** at `Source/TD_6/Private/<SystemFolder>/<ClassName>.cpp`:
   - `#include "<SystemFolder>/<ClassName>.h"` as first include
   - If overriding `BeginPlay`, always call `Super::BeginPlay()`
   - If the class has replicated properties, include `GetLifetimeReplicatedProps` override with `#include "Net/UnrealNetwork.h"`

4. **Validation** — Before writing, check against `references/validations.md`:
   - All `UObject*` pointers must have `UPROPERTY()`
   - `GENERATED_BODY()` present
   - `.generated.h` is last include
   - `#pragma once` present
   - `BlueprintCallable` functions have `Category`

## Project Conventions

- API macro: `TD_6_API`
- Use `TObjectPtr<>` for object pointer properties (e.g., `TObjectPtr<class UButton>`)
- Use tabs for indentation
- Widget bound properties use `UPROPERTY(meta = (BindWidget))`
- Delegates declared with `DECLARE_DYNAMIC_MULTICAST_DELEGATE` macros
- Module dependencies in `TD_6.Build.cs`: Core, CoreUObject, Engine, InputCore, EnhancedInput, Slate, SlateCore, OnlineSubsystem, OnlineSubsystemUtils, NetCore
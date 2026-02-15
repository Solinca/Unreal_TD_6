---
name: ue-validate
description: Validate C++ source files against project rules from references/validations.md
user-invocable: true
---

# Validate UE Code

Scan project C++ files for common Unreal Engine mistakes and anti-patterns.

## Instructions

1. Ask the user which files to validate. Options:
   - A specific file path
   - A system folder (e.g., `UI`, `Network`, `Global`, `Player`)
   - `all` — scan all `.h` and `.cpp` files under `Source/TD_6/`

2. Read `references/validations.md` for the full list of validation rules.

3. For each target file, check for these violations (ordered by severity):

   **Critical:**
   - Raw UObject pointers without `UPROPERTY`
   - World access in constructors (`GetWorld()`, `SpawnActor`, etc.)
   - Missing `GENERATED_BODY()` in UCLASS
   - Missing `#pragma once` in headers

   **Error:**
   - Unchecked `Cast<>()` result used directly with `->`
   - `GetAllActorsOfClass` inside `Tick`
   - Multicast RPC inside `Tick`
   - `BeginPlay` override without `Super::BeginPlay()`
   - `EndPlay` override without `Super::EndPlay()`
   - Missing `GetLifetimeReplicatedProps` for replicated properties

   **Warning:**
   - Server RPC without `WithValidation`
   - Hard asset references (direct `UStaticMesh*`, `USkeletalMesh*`, etc.)
   - Synchronous asset loading during gameplay
   - Replicated property modified without `HasAuthority()` check
   - `LoadObject` with path string
   - `BlueprintCallable` without `Category`

   **Info:**
   - `PrimaryActorTick.bCanEverTick = true` (is it needed?)
   - Magic numbers in timers
   - Hardcoded strings in `UE_LOG` (should use `TEXT()`)
   - `.generated.h` not last include

4. Report findings grouped by file, with:
   - Severity level
   - Line number
   - Rule ID
   - The offending code
   - Suggested fix

5. Summarize total counts by severity at the end.
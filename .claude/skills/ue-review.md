---
name: ue-review
description: Review UE C++ code for sharp edges, anti-patterns, and replication issues
user-invocable: true
---

# Review UE Code

Perform a thorough code review of Unreal Engine C++ files against project guidelines.

## Instructions

1. Ask the user which files or system to review (specific file, folder, or recent git changes).

2. Read the target files, then review against these sources of truth:
   - `references/sharp_edges.md` — critical failure patterns
   - `references/patterns.md` — recommended patterns and anti-patterns
   - `references/validations.md` — strict validation rules

3. Check for **Sharp Edges** (from `references/sharp_edges.md`):
   - UPROPERTY missing on UObject pointers
   - Tick abuse (logic that should be event-driven)
   - Replication authority confusion (missing `HasAuthority()`)
   - Hard asset references instead of soft references
   - Constructor component/world access
   - Unchecked casts
   - `GetAllActorsOfClass` spam
   - RPC bandwidth issues (multicast in tick, large structs)
   - BeginPlay order dependencies

4. Check for **Anti-Patterns** (from `references/patterns.md`):
   - Deep inheritance instead of composition (Actor Components)
   - Fighting the Gameplay Framework (wrong class for wrong job)
   - Blueprint spaghetti indicators
   - Synchronous asset loading

5. Check for **Multiplayer correctness** (this is a networked game):
   - Authority checks before modifying replicated state
   - Proper RPC usage (Server for client requests, Client for server responses)
   - `WithValidation` on Server RPCs
   - `GetLifetimeReplicatedProps` implemented for replicated properties
   - `DOREPLIFETIME` macros present
   - Proper use of `ReplicatedUsing` with `OnRep_` functions

6. Present findings as:
   - **Issues** — things that are wrong or dangerous
   - **Suggestions** — improvements that follow project patterns
   - **Positive** — things done well (briefly)

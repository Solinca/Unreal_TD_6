---
name: ue-replicate
description: Add network replication to an existing class — replicated properties, RPCs, and OnRep functions
user-invocable: true
---

# Add Replication

Add network replication support to an existing C++ class in this multiplayer project.

## Instructions

1. Ask the user for:
   - **Target class** — which class to add replication to
   - **What to replicate** — properties, RPCs, or both
   - For properties: name, type, and whether they need `OnRep` callbacks
   - For RPCs: Server, Client, or Multicast; Reliable or Unreliable

2. Read the target class header and source files.

3. For **Replicated Properties**, add:

   In the header:
   ```cpp
   // Simple replication
   UPROPERTY(Replicated)
   int32 PropertyName;

   // With RepNotify
   UPROPERTY(ReplicatedUsing = OnRep_PropertyName)
   int32 PropertyName;

   UFUNCTION()
   void OnRep_PropertyName();
   ```

   In the source, ensure `GetLifetimeReplicatedProps` exists:
   ```cpp
   #include "Net/UnrealNetwork.h"

   void AClassName::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
   {
       Super::GetLifetimeReplicatedProps(OutLifetimeProps);
       DOREPLIFETIME(AClassName, PropertyName);
       // Or with conditions:
       // DOREPLIFETIME_CONDITION(AClassName, PropertyName, COND_OwnerOnly);
   }
   ```

   Also ensure the constructor has: `bReplicates = true;`

4. For **RPCs**, add:

   Server RPC (client requests action from server):
   ```cpp
   // Header
   UFUNCTION(Server, Reliable, WithValidation)
   void Server_ActionName(params);

   // Source
   bool AClassName::Server_ActionName_Validate(params)
   {
       return true; // Add validation logic
   }

   void AClassName::Server_ActionName_Implementation(params)
   {
       // Server-side logic
   }
   ```

   Client RPC (server tells specific client):
   ```cpp
   UFUNCTION(Client, Reliable)
   void Client_ActionName(params);
   ```

   Multicast RPC (server tells all clients — use sparingly):
   ```cpp
   UFUNCTION(NetMulticast, Unreliable)  // Unreliable for cosmetic effects
   void Multicast_ActionName(params);
   ```

5. **Validation checklist** (from `references/validations.md` and `references/sharp_edges.md`):
   - Server RPCs MUST have `WithValidation`
   - Replicated properties MUST have `GetLifetimeReplicatedProps` with `DOREPLIFETIME`
   - Always check `HasAuthority()` before modifying replicated state
   - Use `FVector_NetQuantize` instead of `FVector` for network vectors
   - Prefer replicated properties over Multicast RPCs for state
   - Never call Multicast RPCs in Tick

#include "Network/MyOnlineBeaconClient.h"
#include "Network/MyOnlineBeaconHostObject.h"

void AMyOnlineBeaconClient::OnConnected()
{
	Super::OnConnected();

	Server_RequestReservation(GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId());
}

void AMyOnlineBeaconClient::Server_RequestReservation_Implementation(const FUniqueNetIdRepl& PlayerNetId)
{
	AMyOnlineBeaconHostObject* Host = Cast<AMyOnlineBeaconHostObject>(GetBeaconOwner());

	if (!Host || Host->ReservedSlots >= Host->MaxSlots)
	{
		Client_ReservationRejected();

		return;
	}

	Host->ReservedSlots++;

	Client_ReservationAccepted();
}

void AMyOnlineBeaconClient::Client_ReservationAccepted_Implementation()
{
	OnRequestValidate.Execute(true);
}

void AMyOnlineBeaconClient::Client_ReservationRejected_Implementation()
{
	OnRequestValidate.Execute(false);
}

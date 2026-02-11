#include "Network/MyOnlineBeaconClient.h"
#include "Network/MyOnlineBeaconHostObject.h"

void AMyOnlineBeaconClient::OnConnected()
{
	Super::OnConnected();

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "CONNECTED TO HOST BEACON");

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	Server_RequestReservation(LocalPlayer->GetPreferredUniqueNetId());
}

void AMyOnlineBeaconClient::OnFailure()
{
	Super::OnFailure();

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "FAILED TO CONNECT TO HOST BEACON");
}

bool AMyOnlineBeaconClient::ConnectToServer(FURL& Url)
{
	return InitClient(Url);
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
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "RESERVATION ACCEPTED");

	OnRequestValidate.Execute(true);
}

void AMyOnlineBeaconClient::Client_ReservationRejected_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "RESERVATION REJECTED");

	OnRequestValidate.Execute(false);
}

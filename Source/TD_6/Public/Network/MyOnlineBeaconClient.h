#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconClient.h"
#include "MyOnlineBeaconClient.generated.h"

DECLARE_DELEGATE_OneParam(FOnRequestValidateSignature, bool);

UCLASS()
class TD_6_API AMyOnlineBeaconClient : public AOnlineBeaconClient
{
	GENERATED_BODY()

public:
	virtual void OnConnected() override;

	virtual void OnFailure() override;

	bool ConnectToServer(FURL& Url);

	UFUNCTION(Server, Reliable)
	void Server_RequestReservation(const FUniqueNetIdRepl& PlayerNetId);

	UFUNCTION(Client, Reliable)
	void Client_ReservationAccepted();

	UFUNCTION(Client, Reliable)
	void Client_ReservationRejected();

	FOnRequestValidateSignature OnRequestValidate;
};

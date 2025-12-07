#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomActor.generated.h"

UCLASS()
class DRAGULARIDER_API ARoomActor : public AActor
{
	GENERATED_BODY()

public:
	ARoomActor();

	// Camerele adiacente acestei camere
	UPROPERTY(EditAnywhere, Category="Room")
	TArray<ARoomActor*> AdjacentRooms;

	// Pentru debug
	UPROPERTY(EditAnywhere)
	FString RoomName;
};

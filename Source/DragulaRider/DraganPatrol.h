// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DraganCharacter.h"
#include "DraganPatrol.generated.h"

/**
 * 
 */
UCLASS()
class DRAGULARIDER_API ADraganPatrol : public ADraganCharacter
{
	
	GENERATED_BODY()

public:
	virtual void Patrol() override;
	virtual void TeleportNearPlayer() override {}
	virtual void ApplyStun(float Duration) override {}

protected:
	UPROPERTY(EditAnywhere)
	TArray<AActor*> PatrolRooms;

	int32 CurrentRoomIndex = 0;
};

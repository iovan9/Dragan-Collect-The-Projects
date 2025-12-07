// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DraganPatrol.h"
#include "DraganTp.generated.h"

UCLASS()
class DRAGULARIDER_API ADraganTp : public ADraganCharacter
{
	GENERATED_BODY()

public:
	virtual void TeleportNearPlayer() override;
	virtual void Patrol() override {}
	virtual void ApplyStun(float Duration) override {}

protected:
	UPROPERTY(EditAnywhere)
	TArray<AActor*> TeleportRooms;
	
};

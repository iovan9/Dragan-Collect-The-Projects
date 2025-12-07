// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DraganCharacter.h"
#include "DraganStunnable.generated.h"

/**
 * 
 */
UCLASS()
class DRAGULARIDER_API ADraganStunnable : public ADraganCharacter
{
	GENERATED_BODY()

public:
	virtual void ApplyStun(float Duration) override;
	virtual void Patrol() override {}
	virtual void TeleportNearPlayer() override {}

protected:
	FTimerHandle StunTimer;
	void ClearStun();
};

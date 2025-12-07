// Fill out your copyright notice in the Description page of Project Settings.


#include "DraganStunnable.h"
#include "TimerManager.h"

void ADraganStunnable::ApplyStun(float Duration)
{
	if (Duration <= 0.f) return;

	bIsStunned = true;

	// Pornim timer pentru dezactivarea stun-ului
	GetWorldTimerManager().SetTimer(
		StunTimer,
		this,
		&ADraganStunnable::ClearStun,
		Duration,
		false
	);
}

void ADraganStunnable::ClearStun()
{
	bIsStunned = false;
}

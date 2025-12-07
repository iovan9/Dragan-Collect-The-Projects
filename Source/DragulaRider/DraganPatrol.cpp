// Fill out your copyright notice in the Description page of Project Settings.


#include "DraganPatrol.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

void ADraganPatrol::Patrol()
{
	if (bIsStunned || PatrolRooms.Num() == 0)
		return;

	AActor* TargetRoom = PatrolRooms[CurrentRoomIndex];
	if (!TargetRoom) return;

	FVector TargetLocation = TargetRoom->GetActorLocation();

	// Mergem spre camera curentă
	FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
	AddMovementInput(Direction, 1.0f);

	float Distance = FVector::Dist(GetActorLocation(), TargetLocation);

	// Dacă a ajuns la cameră -> trece la următoarea
	if (Distance < 5.f)
	{
		CurrentRoomIndex = (CurrentRoomIndex + 1) % PatrolRooms.Num();
	}
}
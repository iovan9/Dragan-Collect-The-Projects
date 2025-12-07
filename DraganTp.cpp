// Fill out your copyright notice in the Description page of Project Settings.


#include "DraganTp.h"

#include "Kismet/GameplayStatics.h"

void ADraganTp::TeleportNearPlayer()
{
	if (bIsStunned || TeleportRooms.Num() == 0)
		return;

	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	FVector PlayerLocation = Player->GetActorLocation();

	// Alegem o cameră *diferită* de cea în care este playerul
	AActor* BestRoom = nullptr;
	float BestDistance = 0.f;

	for (AActor* Room : TeleportRooms)
	{
		if (!Room) continue;

		float Dist = FVector::Dist(Room->GetActorLocation(), PlayerLocation);

		if (Dist > 600.f && Dist > BestDistance)
		{
			BestDistance = Dist;
			BestRoom = Room;
		}
	}

	if (BestRoom)
	{
		FVector TeleportLocation = BestRoom->GetActorLocation() + FVector(80, 80, 0);
		SetActorLocation(TeleportLocation);
	}
}

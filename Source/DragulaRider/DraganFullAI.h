#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RoomActor.h"
#include "DraganFullAI.generated.h"

UENUM(BlueprintType)
enum class EDraganState : uint8
{
	PATROL,
	CHASE,
	SEARCH,
	STUNNED,
	ATTACKING
};


UCLASS()
class DRAGULARIDER_API ADraganFullAI : public ACharacter
{
	GENERATED_BODY()

public:

	EDraganState CurrentState = EDraganState::PATROL;
	ADraganFullAI();

	virtual void Tick(float DeltaTime) override;

	// ---- PATRULARE ----
	UPROPERTY(EditAnywhere, Category="AI|Patrol")
	TArray<AActor*> PatrolRooms;

	int32 CurrentRoomIndex = 0;
	int32 LastVisitedPatrolIndex = -1;

	bool bIsWaiting = false;
	bool bIsIdle = false;
	bool bIsStunned = false;


	void AdvancePatrolIndex();
	FVector GetNextPatrolPoint() const;
	void EndWait();
	void EndIdle();

	//--CHASE--//

	float SightRange = 3500.f;        // camere mari → range mare
	float LoseSightRange = 4500.f;    // ca să nu facă flicker
	float FOV = 120.f;

	FVector LastKnownPlayerLocation;
	float SearchDuration = 3.0f;
	FTimerHandle SearchTimerHandle;
	
	// ---- TELEPORT ----
	UPROPERTY(EditAnywhere, Category="AI|Rooms")
	TArray<ARoomActor*> AllRooms;

	ARoomActor* GetPlayerRoom() const;
	void TeleportNearPlayer();

	void SetPatrolIndexToClosestPoint();

	int32 RoomsVisitedSinceTeleport = 0;
	
	// ---- STUN ----
	UFUNCTION(BlueprintCallable)
	void ApplyStun();
	
	void ClearStun();

	FORCEINLINE bool IsWaiting() const { return bIsWaiting; }
	FORCEINLINE bool IsIdle() const { return bIsIdle; }
	FORCEINLINE bool IsStunned() const { return bIsStunned; }

	//		DMG		//
	
	// ca să știm dacă atacul deja s-a executat
	bool bHasAttacked = false;

	// timer pentru reset la atac
	FTimerHandle AttackResetTimer;

	// range-ul de atac
	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackRange = 250.f;

	// damage
	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackDamage = 1.f;

	void FinishAttack();


	
private:
	FTimerHandle WaitTimer;
	FTimerHandle IdleTimer;
	FTimerHandle StunTimer;
};

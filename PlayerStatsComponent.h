#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatsComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DRAGULARIDER_API UPlayerStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:    
	UPlayerStatsComponent();

	//EVENTS//
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintStart);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintStop);

	UPROPERTY(BlueprintAssignable, Category="Sprint")
	FOnSprintStart OnSprintStart;

	UPROPERTY(BlueprintAssignable, Category="Sprint")
	FOnSprintStop OnSprintStop;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStunActivated);

	UPROPERTY(BlueprintAssignable, Category = "Stun")
	FOnStunActivated OnStunActivated;


	
protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// --------------------
	//      STATS
	// --------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float HP = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	int32 Acad = 0;

	// --------------------
	//      STAMINA
	// --------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
	float Stamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
	float DrainRate = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
	float RegenRate = 15.f;

	UPROPERTY(BlueprintReadOnly, Category="Stamina")
	bool bIsSprinting = false;

	// --------------------
	//			STUN
	// --------------------
	
	UPROPERTY(EditAnywhere)
	float StunCooldown = 12.f;
	
	bool bCanStun = true;
	FTimerHandle StunCooldownHandle;



	// --------------------
	// FUNCTIONS
	// --------------------

	UFUNCTION(BlueprintCallable)
	void AddAcad();


	UFUNCTION(BlueprintCallable)
	void ReduceHP(float Amount);

	UFUNCTION(BlueprintCallable)
	void Heal(float Amount);

	UFUNCTION(BlueprintCallable)
	bool StartSprint();

	UFUNCTION(BlueprintCallable)
	void StopSprint();

	UFUNCTION(BlueprintCallable)
	bool isSprinting() const	{ return bIsSprinting; }
	
	UFUNCTION(BlueprintPure)
	bool IsDead() const { return HP <= 0; }

	UFUNCTION(BlueprintPure)
	bool CanWin() const { return Acad >= 8; }

	UFUNCTION(BlueprintCallable, Category="Stun")
	void ActivateStun();

	UFUNCTION(BlueprintCallable, Category="Stun")
	bool canStun() const { return bCanStun; }

	UFUNCTION(BlueprintCallable)
	void ApplyDamage(float Amount);


};

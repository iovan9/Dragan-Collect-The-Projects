#include "PlayerStatsComponent.h"
#include "DraganException.h"

UPlayerStatsComponent::UPlayerStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UPlayerStatsComponent::BeginPlay()
{	
	Super::BeginPlay();
}

void UPlayerStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsSprinting)
	{
		Stamina -= DrainRate * DeltaTime;

		if (Stamina <= 0.f)
		{
			Stamina = 0.f;
			StopSprint();
		}
	}
	else
	{
		Stamina = FMath::Clamp(Stamina + RegenRate * DeltaTime, 0.f, MaxStamina);
	}
}

// -----------------
//  Functions
// -----------------

void UPlayerStatsComponent::AddAcad()
{
	Acad++;
}

void UPlayerStatsComponent::ReduceHP(float Amount)
{
	HP -= Amount;
	if (HP < 0.f)
		HP = 0.f;
}

void UPlayerStatsComponent::Heal(float Amount)
{
	HP = FMath::Clamp(HP + Amount, 0.f, 3.f);
}

bool UPlayerStatsComponent::StartSprint()
{
	if (Stamina <= 0.f)
		throw StaminaDepleted(); 

	bIsSprinting = true;
	OnSprintStart.Broadcast();
	return true;
}

void UPlayerStatsComponent::StopSprint()
{
	bIsSprinting = false;
	OnSprintStop.Broadcast();
}

void UPlayerStatsComponent::ActivateStun()
{
	// dacă stun-ul e pe cooldown, nu facem nimic
	if (!bCanStun)
		return;

	// trimitem event-ul către Blueprint să stuneze Dragan
	OnStunActivated.Broadcast();

	// pornim cooldown-ul
	bCanStun = false;

	GetWorld()->GetTimerManager().SetTimer(
		StunCooldownHandle,
		[this]() { bCanStun = true; },
		StunCooldown,
		false
	);
}

void UPlayerStatsComponent::ApplyDamage(float Amount)
{
	HP -= Amount;

	if (HP < 0.f)
		HP = 0.f;

	UE_LOG(LogTemp, Warning, TEXT("Player HP: %f"), HP);

	// poți pune un event pentru moarte
	if (HP <= 0.f)
	{
		// death logic or event dispatch
		UE_LOG(LogTemp, Error, TEXT("PLAYER DIED"));
	}
}

#include "DraganAIController.h"
#include "DraganFullAI.h"
#include "DraganException.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ADraganAIController::ADraganAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ADraganAIController::OnPlayerStunActivated()
{
    if (ControlledMonster && !ControlledMonster->IsStunned())
    {
        ControlledMonster->ApplyStun();
    }
}


void ADraganAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ControlledMonster = Cast<ADraganFullAI>(InPawn);

    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Player)
    {
        UPlayerStatsComponent* Stats = Player->FindComponentByClass<UPlayerStatsComponent>();
        if (Stats)
        {
            Stats->OnStunActivated.AddDynamic(this, &ADraganAIController::OnPlayerStunActivated);
        }
    }
}



void ADraganAIController::HandlePatrol()
{
    // dacă stă, e idle sau stun → nu face nimic
    if (ControlledMonster->IsWaiting() || ControlledMonster->IsIdle() || ControlledMonster->IsStunned())
        return;

    ControlledMonster->GetCharacterMovement()->MaxWalkSpeed = 300;

    FVector Target = ControlledMonster->GetNextPatrolPoint();
    FVector MyLoc = ControlledMonster->GetActorLocation();

    float Dist = FVector::Dist(MyLoc, Target);

    // Dacă a ajuns la punct -> intră în WAIT
    if (Dist < 150.f)
    {
        ControlledMonster->AdvancePatrolIndex();
        return;
    }
    // face rotate natural către punctul de patrulare
    FVector ToPoint = Target - ControlledMonster->GetActorLocation();
    ToPoint.Z = 0;
    FRotator DesiredRot = ToPoint.Rotation();

    ControlledMonster->SetActorRotation(
        FMath::RInterpTo(
            ControlledMonster->GetActorRotation(),
            DesiredRot,
            GetWorld()->GetDeltaSeconds(),
            5.0f
        )
    );

    // continuă patrularea
    MoveToLocation(Target);
}

void ADraganAIController::HandleChase()
{
    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return;

    ControlledMonster->GetCharacterMovement()->MaxWalkSpeed = 550.f;

    float Dist = FVector::Dist(
        ControlledMonster->GetActorLocation(),Player->GetActorLocation());
    
    if (Dist <= ControlledMonster->AttackRange)
    {
        StopMovement();
        ControlledMonster->CurrentState = EDraganState::ATTACKING;
        return;
    }

    // orientare naturală spre player
    FVector ToPlayer = Player->GetActorLocation() - ControlledMonster->GetActorLocation();
    ToPlayer.Z = 0;
    FRotator NewRot = ToPlayer.Rotation();
    ControlledMonster->SetActorRotation(
        FMath::RInterpTo(
            ControlledMonster->GetActorRotation(),
            NewRot,
            GetWorld()->GetDeltaSeconds(),
            8.0f
        )
    );

    // urmărește player-ul pe navmesh
    MoveToActor(Player, 5.0f);
}




void ADraganAIController::HandleSearch()
{
    ControlledMonster->GetCharacterMovement()->MaxWalkSpeed = 350;

    // du-te la ultima poziție unde l-ai văzut
    MoveToLocation(ControlledMonster->LastKnownPlayerLocation);

    float Dist = FVector::Dist(
        ControlledMonster->GetActorLocation(),
        ControlledMonster->LastKnownPlayerLocation
    );

    // dacă ai ajuns acolo → caută câteva secunde
    if (Dist < 200.f)
    {
        ControlledMonster->CurrentState = EDraganState::PATROL;
    }
}

void ADraganAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    try
    {
        if (!ControlledMonster || ControlledMonster->IsStunned())
            return;

        switch (ControlledMonster->CurrentState)
        {
        case EDraganState::PATROL:
            if (!ControlledMonster->IsIdle() && !ControlledMonster->IsWaiting())
                HandlePatrol();
            break;

        case EDraganState::CHASE:
            HandleChase();
            break;

        case EDraganState::SEARCH:
            HandleSearch();
            break;

        case EDraganState::ATTACKING:
        case EDraganState::STUNNED:
            return;
        }
    }
    catch (const DraganException& e)
    {
        UE_LOG(LogTemp, Error, TEXT("AI Exception: %s"), *FString(e.what()));
    }
}
 


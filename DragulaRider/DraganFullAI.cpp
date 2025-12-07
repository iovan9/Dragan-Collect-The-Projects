#include "DraganFullAI.h"
#include "DraganException.h"
#include "PlayerStatsComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"

ADraganFullAI::ADraganFullAI()
{
    PrimaryActorTick.bCanEverTick = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0, 600.f, 0);

}

void ADraganFullAI::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsStunned)
        return;

    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return;

    FVector MyLoc = GetActorLocation();
    FVector PlayerLoc = Player->GetActorLocation();
    FVector ToPlayer = PlayerLoc - MyLoc;
    float DistToPlayer = ToPlayer.Size();

    // ---- 1. Distanță ----
    if (DistToPlayer < SightRange)
    {
        // 2. FOV mare
        FVector Forward = GetActorForwardVector();
        float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(Forward, ToPlayer.GetSafeNormal())));

        if (Angle < FOV)
        {
            // 3. SphereTrace (vede playerul în 99% din cazuri)
            FHitResult Hit;
            FCollisionQueryParams Params;
            Params.AddIgnoredActor(this);

            bool bHit = GetWorld()->SweepSingleByChannel(
                Hit,
                MyLoc + FVector(0, 0, 50),
                PlayerLoc,
                FQuat::Identity,
                ECC_Visibility,
                FCollisionShape::MakeSphere(50.f),
                Params
            );

            if (bHit && Hit.GetActor() == Player)
            {
                CurrentState = EDraganState::CHASE;
                LastKnownPlayerLocation = PlayerLoc;
                return;
            }
        }
    }

    // Dacă era în chase și te pierde:
    if (CurrentState == EDraganState::CHASE && DistToPlayer > LoseSightRange)
    {
        CurrentState = EDraganState::SEARCH;
        LastKnownPlayerLocation = PlayerLoc;
        return;
    }
    
    if (CurrentState == EDraganState::ATTACKING)
    {
        // oprește mișcarea complet
        GetCharacterMovement()->DisableMovement();

        if (!bHasAttacked)
        {
            bHasAttacked = true;

            if (Player)
            {
                if (auto* Stats = Cast<UPlayerStatsComponent>(Player->FindComponentByClass<UActorComponent>()))
                {
                    Stats->ApplyDamage(AttackDamage);
                    UE_LOG(LogTemp, Warning, TEXT("Dynamic cast reușit → aplic damage prin component!"));
                }

            }

            // stă 4 secunde după atac → Idle/Recovery
            GetWorld()->GetTimerManager().SetTimer(
                AttackResetTimer,
                this,
                &ADraganFullAI::FinishAttack,
                4.0f,
                false
            );
        }

        return; // nu continuă restul logicii
    }


}


FVector ADraganFullAI::GetNextPatrolPoint() const
{
    if (PatrolRooms.Num() == 0)
        return GetActorLocation();

    return PatrolRooms[CurrentRoomIndex]->GetActorLocation();
}

void ADraganFullAI::AdvancePatrolIndex()
{
    if (bIsWaiting || bIsIdle || bIsStunned)
        return;

    // Crestere camere vizitate
    RoomsVisitedSinceTeleport++;

    if (RoomsVisitedSinceTeleport >= 4)
    {
        RoomsVisitedSinceTeleport = 0;
        TeleportNearPlayer();
        return;
    }

    
    int32 NextIndex = (CurrentRoomIndex + 1) % PatrolRooms.Num();

    // Anti loop: evită să meargă înapoi la punctul anterior
    if (NextIndex == LastVisitedPatrolIndex)
    {
        NextIndex = (NextIndex + 1) % PatrolRooms.Num();
    }

    LastVisitedPatrolIndex = CurrentRoomIndex;
    CurrentRoomIndex = NextIndex;

    bIsWaiting = true;

    GetWorldTimerManager().SetTimer(
        WaitTimer,
        this,
        &ADraganFullAI::EndWait,
        3.0f,
        false
    );
}

void ADraganFullAI::EndWait()
{
    bIsWaiting = false;
}



void ADraganFullAI::SetPatrolIndexToClosestPoint()
{
    if (PatrolRooms.Num() == 0)
        return;
    
    FVector Loc = GetActorLocation();

    float BestDist = FLT_MAX;
    int32 BestIndex = 0;

    for (int32 i = 0; i < PatrolRooms.Num(); i++)
    {
        AActor* P = PatrolRooms[i];
        if (!P) continue;

        float Dist = FVector::Dist(Loc, P->GetActorLocation());
        
        // evită automat punctul anterior
        if (i == LastVisitedPatrolIndex)
            continue;

        if (Dist < BestDist)
        {
            BestDist = Dist;
            BestIndex = i;
        }
    }

    CurrentRoomIndex = BestIndex;
}

ARoomActor* ADraganFullAI::GetPlayerRoom() const
{
    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return nullptr;

    FVector PlayerLoc = Player->GetActorLocation();

    ARoomActor* BestRoom = nullptr;
    float BestDist = FLT_MAX;

    for (ARoomActor* Room : AllRooms)
    {
        if (!Room) continue;

        float Dist = FVector::Dist(PlayerLoc, Room->GetActorLocation());

        if (Dist < BestDist)
        {
            BestDist = Dist;
            BestRoom = Room;
        }
    }

    return BestRoom;
}

void ADraganFullAI::TeleportNearPlayer()
{
    if (bIsStunned || bIsIdle)
        return;

    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return;

    FVector PlayerLoc = Player->GetActorLocation();
    FVector MonsterLoc = GetActorLocation();

    ARoomActor* PlayerRoom = GetPlayerRoom();
    if (!PlayerRoom) return;

    TArray<ARoomActor*> Adjacent = PlayerRoom->AdjacentRooms;
    if (Adjacent.Num() == 0) throw InvalidTeleportRoom();

    ARoomActor* BestRoom = nullptr;
    float BestScore = FLT_MAX;

    for (ARoomActor* Room : Adjacent)
    {
        if (!Room) continue;

        FVector RoomLoc = Room->GetActorLocation();

        float DistToPlayer = FVector::Dist(RoomLoc, PlayerLoc);
        if (DistToPlayer < 1000.f) // camerele tale sunt mari — 1000 e minim realist
            continue;

        // Anti vizibilitate (player cannot see teleport location)
        FHitResult Hit;
        FVector Start = PlayerLoc + FVector(0,0,50);
        FVector End = RoomLoc;

        FCollisionQueryParams Params;
        Params.AddIgnoredActor(Player);
        Params.AddIgnoredActor(this);

        bool bSaw = !GetWorld()->LineTraceSingleByChannel(
            Hit, Start, End, ECC_Visibility, Params
        );

        if (bSaw)
            continue;

        // Alegem camera cea mai apropiată de monstrul actual
        float DistToMonster = FVector::Dist(RoomLoc, MonsterLoc);

        if (DistToMonster < BestScore)
        {
            BestScore = DistToMonster;
            BestRoom = Room;
        }
    }

    if (BestRoom)
    {
        UE_LOG(LogTemp, Warning, TEXT("DRAGAN TELEPORT → %s"), *BestRoom->GetName());
        SetActorLocation(BestRoom->GetActorLocation());

        // reset patrulare la punctul cel mai apropiat
        SetPatrolIndexToClosestPoint();
        LastVisitedPatrolIndex = -1;

        // idle 3 secunde
        bIsIdle = true;
        GetWorldTimerManager().SetTimer(
            IdleTimer,
            this,
            &ADraganFullAI::EndIdle,
            4.0f,
            false
        );
    }
}

void ADraganFullAI::EndIdle()
{
    bIsIdle = false;
}

void ADraganFullAI::ApplyStun()
{
    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return;

    if (bIsStunned) throw StunAlreadyActive();

    FVector Start = Player->GetActorLocation() + FVector(0,0,60);
    FVector Forward = Player->GetControlRotation().Vector();
    FVector End = Start + Forward * 2000.f; // RANGE

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Player);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    // daca am lovit cv
    if (!bHit) return;

    // daca e dragan
    if (Hit.GetActor() != this) return;

    //range
    float Dist = FVector::Dist(Player->GetActorLocation(), GetActorLocation());
    if (Dist > 2500.f) return;

    //fov
    FVector ToDragan = GetActorLocation() - Player->GetActorLocation();
    ToDragan.Z = 0;
    ToDragan.Normalize();

    float Dot = FVector::DotProduct(Forward, ToDragan);
    if (Dot < 0.7f) return; // ~45°
    
    bIsStunned = true;
    CurrentState = EDraganState::STUNNED;

    GetCharacterMovement()->DisableMovement();

    GetWorldTimerManager().SetTimer(
        StunTimer,
        this,
        &ADraganFullAI::ClearStun,
        3.0f,
        false
    );
}

void ADraganFullAI::ClearStun()
{
    UE_LOG(LogTemp, Warning, TEXT("DRAGAN STUN CLEARED"));

    bIsStunned = false;
    CurrentState = EDraganState::PATROL;

    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void ADraganFullAI::FinishAttack()
{
    bHasAttacked = false;

    // revine la chase
    CurrentState = EDraganState::CHASE;

    // reactivați mișcarea
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DraganCharacter.generated.h"

UCLASS()
class DRAGULARIDER_API ADraganCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ADraganCharacter();
    virtual void Patrol() PURE_VIRTUAL(ADraganMonsterBase::Patrol, );
    virtual void TeleportNearPlayer() PURE_VIRTUAL(ADraganMonsterBase::TeleportNearPlayer, );
    virtual void ApplyStun(float Duration) PURE_VIRTUAL(ADraganMonsterBase::ApplyStun, );

    virtual bool IsStunned() const { return bIsStunned; }

protected:
    bool bIsStunned = false;
};
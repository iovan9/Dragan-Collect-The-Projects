#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "DraganFullAI.h"
#include "PlayerStatsComponent.h"
#include "DraganAIController.generated.h"

UCLASS()
class DRAGULARIDER_API ADraganAIController : public AAIController
{
	GENERATED_BODY()

public:
	ADraganAIController();
	void HandlePatrol();
	void HandleChase();
	void HandleSearch();

	UFUNCTION()
	void OnPlayerStunActivated();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaSeconds) override;

private:
	class ADraganFullAI* ControlledMonster;
};

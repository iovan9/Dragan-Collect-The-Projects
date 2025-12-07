// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlashlightActor.generated.h"

UCLASS()
class DRAGULARIDER_API AFlashlightActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlashlightActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Toggle the flashlight on/off
	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void ToggleFlashlight();

protected:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight")
	class USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight")
	class UStaticMeshComponent* FlashlightMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight")
	class USpotLightComponent* SpotLight;

	// Starting state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
	bool bStartOn = false;

	// Current state
	bool bIsOn = false;
};

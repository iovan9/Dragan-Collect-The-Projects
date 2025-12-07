// Fill out your copyright notice in the Description page of Project Settings.

#include "FlashlightActor.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "InputCoreTypes.h"

AFlashlightActor::AFlashlightActor()
{
    PrimaryActorTick.bCanEverTick = true;

    RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(RootComp);

    FlashlightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashlightMesh"));
    FlashlightMesh->SetupAttachment(RootComp);

    SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
    SpotLight->SetupAttachment(FlashlightMesh);

    // Reasonable defaults for a flashlight
    SpotLight->SetIntensity(5000.0f);
    SpotLight->SetInnerConeAngle(12.0f);
    SpotLight->SetOuterConeAngle(20.0f);
    SpotLight->SetAttenuationRadius(2000.0f);
    SpotLight->bUseInverseSquaredFalloff = false; // easier to tune
    SpotLight->SetVisibility(false);
}

void AFlashlightActor::BeginPlay()
{
    Super::BeginPlay();

    bIsOn = bStartOn;
    SpotLight->SetVisibility(bIsOn);

    // Try to enable input so the actor can toggle itself when placed in the level.
    if (APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
    {
        EnableInput(PC);
        if (InputComponent)
        {
            // Bind to the F key directly so no action mapping is required.
            InputComponent->BindKey(EKeys::F, IE_Pressed, this, &AFlashlightActor::ToggleFlashlight);
        }
    }
}

void AFlashlightActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AFlashlightActor::ToggleFlashlight()
{
    bIsOn = !bIsOn;
    if (SpotLight)
    {
        SpotLight->SetVisibility(bIsOn);
    }

    // Small feedback
    if (GEngine && GetWorld())
    {
        const FString Msg = FString::Printf(TEXT("Flashlight: %s"), bIsOn ? TEXT("On") : TEXT("Off"));
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, Msg);
    }
}
// Fill out your copyright notice in the Description page of Project Settings.


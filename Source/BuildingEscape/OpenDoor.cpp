// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	CurrentYaw = GetOwner()->GetActorRotation().Yaw;
	ClosedYaw = CurrentYaw;
	OpenAngle += ClosedYaw;

	FindPressurePlate();
	FindAudioComponent();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressurePlate && TotalMassOfActors() > MassToOpenDoor)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else 
	{
		if(GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay) 
		{
			CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	CurrentYaw = FMath::Lerp(CurrentYaw, OpenAngle, DeltaTime * DoorOpenSpeed);
	FRotator DoorRotation(0.f, CurrentYaw, 0.f);
	GetOwner()->SetActorRotation(DoorRotation);

	CloseDoorSound = false;
	if(!AudioComponent) {return;}
	if(!OpenDoorSound)
	{
		AudioComponent->Play();
		OpenDoorSound = true;
	}
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentYaw = FMath::Lerp(CurrentYaw, ClosedYaw, DeltaTime * DoorCloseSpeed);
	FRotator DoorRotation(0.f, CurrentYaw, 0.f);
	GetOwner()->SetActorRotation(DoorRotation);

	OpenDoorSound = false;
	if(!AudioComponent) {return;}
	if(!CloseDoorSound)
	{
		AudioComponent->Play();
		CloseDoorSound = true;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	if (!PressurePlate) {return TotalMass;}

	// Find all overlapping actors
	TArray<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	// Add mass of overlapping actors
	for (AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	
	return TotalMass;
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s requires an Audio Component but none is attached"), *GetOwner()->GetName());
	}
}

void UOpenDoor::FindPressurePlate() const
{
	if(!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the open door component but no PressurePlate is set!"), *GetOwner()->GetName());
	}
}
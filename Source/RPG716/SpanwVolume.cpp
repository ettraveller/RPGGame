// Fill out your copyright notice in the Description page of Project Settings.


#include "SpanwVolume.h"
#include <Components/BoxComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include "Engine/World.h"
#include "Critter.h"

// Sets default values
ASpanwVolume::ASpanwVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Spanwing Box"));

}

// Called when the game starts or when spawned
void ASpanwVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpanwVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpanwVolume::GetSpawnPoint()
{
    FVector Extent = SpawnBox->GetScaledBoxExtent();
	FVector Origin = SpawnBox->GetComponentLocation();

	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);

	return Point;
} 

void ASpanwVolume::SpawnOurPawn_Implementation(UClass* ToSpawn, const FVector& Location)
{
	if (ToSpawn)
	{
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;

		if (World)
		{
			ACritter* CritterSpawned = World->SpawnActor<ACritter>(ToSpawn, Location, FRotator(0.f), SpawnParams);
		}
	}
}


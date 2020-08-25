// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(TileMesh);
}

void ATile::SetNum(int num)
{
	if(lblNum == nullptr)
	{
		return;
	}
	
	lblNum->SetText(FText::AsNumber(num));
}

void ATile::SetActive(const bool activate) 
{
	SetActorHiddenInGame(!activate);
	//SetActorTickEnabled(activate);
}

void ATile::BeginPlay()
{
	Super::BeginPlay();
	
	lblNum = this->FindComponentByClass<UTextRenderComponent>();
}

void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


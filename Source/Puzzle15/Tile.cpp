// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = true;
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(TileMesh);

	_latentInfo.CallbackTarget = this;
}

int ATile::GetNum()
{
	return _num;
}

void ATile::Initialize(int num, float* width)
{
	if(lblNum == nullptr)
	{
		return;
	}
	
	lblNum->SetText(FText::AsNumber(num));
	_num = num;
	_width = *width;
}

void ATile::MoveTo(FVector dir)
{
	UE_LOG(LogTemp, Warning, TEXT("dir: %s"), *dir.ToString());
	
	UKismetSystemLibrary::MoveComponentTo(
		TileMesh,
		TileMesh->GetRelativeLocation() + dir * _width,
		TileMesh->GetRelativeRotation(),
		false,
		false,
		1,
		false,
		EMoveComponentAction::Move,
		_latentInfo);
}

void ATile::SetActive(const bool activate) 
{
	SetActorHiddenInGame(!activate);
	SetActorTickEnabled(activate);
}

void ATile::MoveStateTick(float deltaTime)
{
	auto pos = GetActorLocation();
	SetActorLocation(pos + FVector::UpVector * deltaTime * 100);
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


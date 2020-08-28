#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/KismetSystemLibrary.h"

float timeToMove = 0; //todo: to Settings class

ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = true;
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(TileMesh);

	_latentInfo.CallbackTarget = this;
}

int ATile::GetNum() const
{
	return _num;
}

void ATile::Initialize(int num, float& width, float& duration)
{
	if(lblNum == nullptr)
	{
		return;
	}
	
	lblNum->SetText(FText::AsNumber(num));
	_num = num;
	_width = width;
	timeToMove = duration;
}

void ATile::MoveTo(const FVector dir, const bool instant) const
{
	if(instant)
	{
		TileMesh->SetRelativeLocation(TileMesh->GetRelativeLocation() + dir * _width);
	}
	else
	{
		UKismetSystemLibrary::MoveComponentTo(
        TileMesh,
        TileMesh->GetRelativeLocation() + dir * _width,
        TileMesh->GetRelativeRotation(),
        false,
        false,
        timeToMove,
        false,
        EMoveComponentAction::Move,
        _latentInfo);
	}
}

void ATile::SetActive(const bool activate) 
{
	SetActorHiddenInGame(!activate);
	SetActorTickEnabled(activate);
}

void ATile::MoveStateTick(float deltaTime)
{
	SetActorLocation(GetActorLocation() + FVector::UpVector * deltaTime * 100);
}

void ATile::BeginPlay()
{
	Super::BeginPlay();
	
	lblNum = this->FindComponentByClass<UTextRenderComponent>();
}


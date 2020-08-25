// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class UTextRenderComponent;
UCLASS()
class PUZZLE15_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TileMesh;

	void SetNum(int num);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UTextRenderComponent* lblNum;

	UPROPERTY(VisibleInstanceOnly)
	UTextRenderComponent* _displayText;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetActive(bool activate);

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <queue>


#include "Coord.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "GameboardActor.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam("")

UCLASS()
class PUZZLE15_API AGameboardActor : public AActor
{
	GENERATED_BODY()

	static enum MoveDirections
	{
		None,
        Left,
        Right,
        Up,
        Down
    };
	
public:	
	AGameboardActor();
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* boardMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Gameboard")
	TSubclassOf<class ATile> tile;

	UFUNCTION(BlueprintCallable)
	void SpawnTiles(int numTiles);

	UFUNCTION()
	void OnSelected(AActor* Target, FKey ButtonPressed);

protected:
	virtual void BeginPlay() override;

private:
	ATile** grid;
	std::queue<ATile*> pool;

	ATile* GetTile();
	void RecycleTile(ATile*);
	Coord* DetermineMoveDir(Coord* hitCoord);
	void Move(Coord* hitCoord, Coord* movement);
	

public:	
	virtual void Tick(float DeltaTime) override;

};

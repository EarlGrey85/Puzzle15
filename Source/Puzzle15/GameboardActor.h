#pragma once

#include <queue>

#include "Coord.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "GameboardActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWinDelegate);

UCLASS()
class PUZZLE15_API AGameboardActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameboardActor();

	UPROPERTY(EditDefaultsOnly)
	float _tileMoveDuration = 0.25f;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* _boardMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Gameboard")
	TSubclassOf<class ATile> _tile;

	UFUNCTION(BlueprintCallable)
	void SpawnTiles(int numTiles);

	UFUNCTION(BlueprintCallable)
	void Shuffle();

	UPROPERTY(BlueprintAssignable)
	FWinDelegate OnWinDelegate;

	void CheckWin();

	UFUNCTION()
	void OnSelected(AActor* Target, FKey ButtonPressed);

	virtual ~AGameboardActor();

protected:
	virtual void BeginPlay() override;

private:
	ATile** _grid;
	std::queue<ATile*> _pool;
	float _tileSize;
	Coord _emptyTileCoord;
	FVector _movementDir;

	ATile* GetTile();
	void RecycleTile(ATile*);
	Coord* DetermineMoveDir(const Coord& hitCoord) const;
	void Move(const Coord& hitCoord, const Coord& movement, const bool instant = false);
};

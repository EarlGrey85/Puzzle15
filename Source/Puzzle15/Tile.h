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
	ATile();
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TileMesh;

	int GetNum() const;	
	void Initialize(int num, float& width, float& duration);
	void MoveTo(const FVector dir, const bool instant = false) const;

protected:
	virtual void BeginPlay() override;

private:
	UTextRenderComponent* lblNum;
	UPROPERTY(VisibleInstanceOnly)
	UTextRenderComponent* _displayText;

	int _num;
	float _width;
	FLatentActionInfo _latentInfo;
	FTransform _transform;

public:	
	void SetActive(bool activate);
	void MoveStateTick(float deltaTime);
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "GameboardActor.h"

#include "Kismet/GameplayStatics.h"

static FVector tileScale;
static FVector boardPosition;
static float boardWidth = 0;
static float boardExtentsZ = 0;
static FTransform transform;

// Sets default values
AGameboardActor::AGameboardActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    boardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    boardMesh->SetupAttachment(RootComponent);
    boardPosition = GetActorLocation();

    transform = GetTransform();
    UE_LOG(LogTemp, Warning, TEXT("%s"), *boardPosition.ToString());
    UE_LOG(LogTemp, Warning, TEXT("%s"), "1");
}

// Called when the game starts or when spawned
void AGameboardActor::BeginPlay()
{
    Super::BeginPlay();
    FVector boardExt;
    GetActorBounds(false, boardPosition, boardExt);
    boardWidth = boardExt.Y * 2;
    boardExtentsZ = boardExt.X;

    this->OnClicked.AddDynamic(this, &AGameboardActor::OnSelected);
    //OnClicked.AddUniqueDynamic(this, &AMyCharacter::OnSelected);
}

// void GetCoordinates(const FVector* hitPos)
// {
//     int column = ((*hitPos).X + GetActorLocation().X - tileSize * initialOffset) / tileSize;
//     int raw = ((*hitPos).Y - GetActorLocation().Y - tileSize * initialOffset + tileSize * 0.5f) / tileSize;
//     UE_LOG(LogTemp, Warning, TEXT("%d:%d"), column, raw);
// }

void AGameboardActor::OnSelected(AActor* Target, FKey ButtonPressed)
{
    FHitResult hit(ForceInit);
    UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHitResultUnderCursor(ECollisionChannel::ECC_WorldDynamic, false, hit);
    auto location = GetTransform().InverseTransformPositionNoScale(hit.ImpactPoint);

    UE_LOG(LogTemp, Warning, TEXT("suka, %s"), "1234");
    
    if (hit.GetActor() != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("suka, %s"), *location.ToString());
    }

    // GetCoordinates(&location);
}

static FVector GetTileSpawnPosition(float tileSize, float initialOffset, float zPos, int raw, int col)
{
    auto pos = FVector::UpVector * (zPos + 3);
    pos.Y -= tileSize * (initialOffset - col);
    pos.X += tileSize * (initialOffset - raw);
    return pos;
}

void AGameboardActor::SpawnTiles(int numTiles)
{
    delete[] grid;
    grid = new ATile *[numTiles * numTiles];

    UE_LOG(LogTemp, Warning, TEXT("%d"), numTiles);
    
    auto ratio = static_cast<float>(1) / static_cast<float>(numTiles) * 0.90f; // * (boardWidth / tileWidth)
    tileScale = FVector(ratio, ratio, ratio);
    auto tileSize = boardWidth / numTiles;
    auto initialOffset = numTiles % 2 == 0 ? numTiles / 2 - 0.5 : numTiles / 2;
    FVector tileExtents = FVector::ZeroVector;
    
    for (int i = 0; i < numTiles * numTiles - 1; ++i)
    {
        auto raw = i / numTiles;
        auto col = i % numTiles;
        
        auto t = GetWorld()->SpawnActor<ATile>(tile, transform);
        t->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true)); 
        t->SetActorRelativeScale3D(tileScale);

        if(tileExtents.IsZero())
        {
            FVector pos = t->GetActorLocation();
            t->GetActorBounds(false, pos, tileExtents);
        }

        const auto tilePos = GetTileSpawnPosition(tileSize, initialOffset, tileExtents.X, raw, col);
        t->SetActorRelativeLocation(tilePos);
        t->SetNum(i + 1);
        grid[i] = t;
    }
}

// Called every frame
void AGameboardActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    float mouseX;
    float mouseY;
    UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetMousePosition(mouseX, mouseY);
    //UE_LOG(LogTemp, Warning, TEXT("Mouse Location: %f, %f"), mouseX, mouseY);
}

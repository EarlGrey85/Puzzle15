#include "GameboardActor.h"
#include "Kismet/GameplayStatics.h"
#include "Coord.h"

static FVector tileScale;
static FVector boardPosition;
static float boardWidth;
static float boardExtentsZ;
static FTransform transform;
static float initialOffset;
static int currentTileCount;
static int numTiles;
//todo: put these above into Settings class

static Coord noMovement = Coord(0, 0);
static Coord rightMove = Coord(1, 0);
static Coord leftMove = Coord(-1, 0);
static Coord upMove = Coord(0, -1);
static Coord downMove = Coord(0, 1);

AGameboardActor::AGameboardActor()
{
    PrimaryActorTick.bCanEverTick = true;
    _boardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    _boardMesh->SetupAttachment(RootComponent);
    boardPosition = GetActorLocation();

    transform = GetTransform();
    UE_LOG(LogTemp, Warning, TEXT("%s"), *boardPosition.ToString());
    UE_LOG(LogTemp, Warning, TEXT("%s"), "1");
   
}

void AGameboardActor::BeginPlay()
{
    Super::BeginPlay();
    FVector boardExt;
    GetActorBounds(false, boardPosition, boardExt);
    boardWidth = boardExt.Y * 2;
    boardExtentsZ = boardExt.X;

    this->OnClicked.AddDynamic(this, &AGameboardActor::OnSelected);
}

Coord GetCoordinates(const FVector* hitPos, const float* tileSize)
{
    const float offset = *tileSize * (initialOffset + 0.5f);
    const int column = ((*hitPos).Y + offset) / *tileSize;
    const int raw = numTiles - ((*hitPos).X + offset) / *tileSize;
    UE_LOG(LogTemp, Warning, TEXT("tileSize: %f"), tileSize);

    return Coord(column, raw);
}

void AGameboardActor::OnSelected(AActor* Target, FKey ButtonPressed)
{
    FHitResult hit(ForceInit);
    UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHitResultUnderCursor(ECC_WorldDynamic, false, hit);
    auto location = GetTransform().InverseTransformPositionNoScale(hit.ImpactPoint);
    auto hitCoord = GetCoordinates(&location, &_tileSize);
    auto movementCoord = DetermineMoveDir(hitCoord);
    UE_LOG(LogTemp, Warning,
        TEXT("hit coord: (%d:%d), movement coord: (%d:%d)"), hitCoord.X, hitCoord.Y, movementCoord.X, movementCoord.Y);

    if(movementCoord == noMovement)
    {
        return;
    }
    
    Move(hitCoord, movementCoord);
}

AGameboardActor::~AGameboardActor()
{
    delete[] _grid;
}

Coord AGameboardActor::DetermineMoveDir(Coord hitCoord)
{
    if(hitCoord.X != _emptyTileCoord.X && hitCoord.Y != _emptyTileCoord.Y)
    {
        return noMovement;
    }

    if(hitCoord.X == _emptyTileCoord.X && hitCoord.Y == _emptyTileCoord.Y)
    {
        return noMovement;
    }

    if(hitCoord.X == _emptyTileCoord.X)
    {
        return  hitCoord.Y < _emptyTileCoord.Y ? downMove : upMove;
    }

    return  hitCoord.X < _emptyTileCoord.X ? rightMove : leftMove;
}

// void helper(int index, int nextEmptyTileIndex, ATile** grid, int offset)
// {
//     while (index != nextEmptyTileIndex)
//     {
//         UE_LOG(LogTemp, Warning, TEXT("move %d:%d"), index, _emptyTileCoord.X);
//         const auto nextIndex = index - offset;
//         grid[nextIndex]->MoveTo(movementDir);
//         grid[index] = grid[nextIndex];
//         index -= offset;
//     }
// }

void AGameboardActor::Move(Coord hitCoord, Coord movement)
{
    _movementDir = FVector(movement.Y, movement.X, 0);
    const auto offset = numTiles * hitCoord.Y;
    auto index = _emptyTileCoord.X + numTiles * _emptyTileCoord.Y;
    const auto nextEmptyTileIndex = hitCoord.X + offset;
    
    if(hitCoord.Y == _emptyTileCoord.Y)
    {
        while (index != nextEmptyTileIndex)
        {
            const auto nextIndex = index - movement.X;
            _grid[nextIndex]->MoveTo(_movementDir);
            _grid[index] = _grid[nextIndex];
            index -= movement.X;
        }
    }
    else if(hitCoord.X == _emptyTileCoord.X)
    {
        while (index != nextEmptyTileIndex)
        {
            const auto nextIndex = index - movement.Y * numTiles;
            _grid[nextIndex]->MoveTo(-_movementDir);
            _grid[index] = _grid[nextIndex];
            index -= movement.Y * numTiles;
        }
    }

    _emptyTileCoord = hitCoord;
    _grid[nextEmptyTileIndex] = nullptr;

    // for (int i = 0; i < numTiles * numTiles; i++)
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("tile %d"), _grid[i] == nullptr ? -1 : _grid[i]->GetNum());
    // }

    CheckWin();
}

static FVector GetTileSpawnPosition(float zPos, int raw, int col, float tileSize)
{
    auto pos = FVector::UpVector * zPos;
    pos.Y -= tileSize * (initialOffset - col);
    pos.X += tileSize * (initialOffset - raw);
    return pos;
}

void AGameboardActor::SpawnTiles(int num)
{
    if (_grid != nullptr)
    {
        for (int i = 0; i < currentTileCount; ++i)
        {
            RecycleTile(_grid[i]);
        }

        delete[] _grid;
    }

    currentTileCount = num * num;
    _grid = new ATile*[currentTileCount]{nullptr};
    numTiles = num;

    const auto ratio = static_cast<float>(1) / static_cast<float>(num) * 0.90f;
    tileScale = FVector(ratio, ratio, ratio);
    _tileSize = boardWidth / num;
    UE_LOG(LogTemp, Warning, TEXT("tileSize %f %d"), _tileSize, num);
    initialOffset = num % 2 == 0 ? num / 2 - 0.5 : num / 2;
    FVector tileExtents = FVector::ZeroVector;

    for (int i = 0; i < num * num - 1; ++i)
    {
        const auto raw = i / num;
        const auto col = i % num;

        auto t = GetTile();
        t->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
        t->SetActorRelativeScale3D(tileScale);

        if (tileExtents.IsZero())
        {
            FVector pos = t->GetActorLocation();
            t->GetActorBounds(false, pos, tileExtents);
        }

        const auto tilePos = GetTileSpawnPosition(tileExtents.X, raw, col, _tileSize);
        t->SetActorRelativeLocation(tilePos);
        t->Initialize(i + 1, _tileSize, _tileMoveDuration);
        _grid[i] = t;
    }
    
    _emptyTileCoord = Coord(num - 1, num - 1);

    UE_LOG(LogTemp, Warning, TEXT("raz %d:%d:%d"), _emptyTileCoord.X, _emptyTileCoord.Y, num);
}

void AGameboardActor::Randomize()
{
    UE_LOG(LogTemp, Warning, TEXT("dva"));
    const auto fakeHit = Coord(_emptyTileCoord.X - 2, _emptyTileCoord.Y);
    const auto fakeMoveCoord = DetermineMoveDir(fakeHit);

    UE_LOG(LogTemp, Warning, TEXT("dvig %d:%d"), fakeMoveCoord.X, fakeMoveCoord.Y);
    
    Move(fakeHit, fakeMoveCoord);
}

void AGameboardActor::CheckWin()
{
    for (int i = 0; i < currentTileCount - 1; ++i)
    {
        if(_grid[i] == nullptr || i != _grid[i]->GetNum() - 1)
        {
            UE_LOG(LogTemp, Warning, TEXT("no win"));
            return;
        }
        
        UE_LOG(LogTemp, Warning, TEXT("dva %d"), _grid[i]->GetNum());
    }

    UE_LOG(LogTemp, Warning, TEXT("win"));
    OnWinDelegate.Broadcast();
}

ATile* AGameboardActor::GetTile()
{
    if (_pool.empty())
    {
        return GetWorld()->SpawnActor<ATile>(_tile, transform);
    }

    const auto t = _pool.front();
    _pool.pop();
    t->SetActive(true);
    return t;
}

void AGameboardActor::RecycleTile(ATile* tile)
{
    if (tile == nullptr)
    {
        return;
    }

    _pool.emplace(tile);
    tile->SetActive(false);
}

#include "GameboardActor.h"

#include "Kismet/GameplayStatics.h"
#include "Coord.h"

static FVector tileScale;
static FVector boardPosition;
static float boardWidth;
static float boardExtentsZ;
static FTransform transform;
static float initialOffset;
static float tileSize;
static int lastTileCount;
static int numTiles;
//todo: put these above into Settings class

static Coord noMovement = Coord(0, 0);
static Coord rightMove = Coord(1, 0);
static Coord leftMove = Coord(-1, 0);
static Coord upMove = Coord(0, -1);
static Coord downMove = Coord(0, 1);

struct Coord emptyTileCoord;
FVector movementDir;

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

Coord GetCoordinates(const FVector* hitPos)
{
    int column = ((*hitPos).Y + tileSize * (initialOffset + 0.5f)) / tileSize;
    int raw = numTiles - ((*hitPos).X + tileSize * (initialOffset + 0.5f)) / tileSize;
    UE_LOG(LogTemp, Warning, TEXT("asdada %f:%d"), tileSize, raw);

    return Coord(column, raw);
}

void AGameboardActor::OnSelected(AActor* Target, FKey ButtonPressed)
{
    FHitResult hit(ForceInit);
    UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHitResultUnderCursor(ECC_WorldDynamic, false, hit);
    auto location = GetTransform().InverseTransformPositionNoScale(hit.ImpactPoint);
    auto hitCoord = GetCoordinates(&location);
    const auto movementCoord = DetermineMoveDir(&hitCoord);

    if(movementCoord == &noMovement)
    {
        return;
    }
    
    movementDir = FVector(movementCoord->Y, movementCoord->X, 0);
    Move(&hitCoord, movementCoord);
}

AGameboardActor::~AGameboardActor()
{
    delete[] _grid;
}

Coord* AGameboardActor::DetermineMoveDir(Coord* hitCoord)
{
    if(hitCoord->X != emptyTileCoord.X && hitCoord->Y != emptyTileCoord.Y)
    {
        return &noMovement;
    }

    if(hitCoord->X == emptyTileCoord.X && hitCoord->Y == emptyTileCoord.Y)
    {
        return &noMovement;
    }

    if(hitCoord->X == emptyTileCoord.X)
    {
        return  hitCoord->Y < emptyTileCoord.Y ? &downMove : &upMove;
    }

    return  hitCoord->X < emptyTileCoord.X ? &rightMove : &leftMove;
}

void helper(int index, int nextEmptyTileIndex, ATile** grid, int offset)
{
    while (index != nextEmptyTileIndex)
    {
        UE_LOG(LogTemp, Warning, TEXT("move %d:%d"), index, emptyTileCoord.X);
        const auto nextIndex = index - offset;
        grid[nextIndex]->MoveTo(movementDir);
        grid[index] = grid[nextIndex];
        index -= offset;
    }
}

void AGameboardActor::Move(Coord* hitCoord, Coord* movement)
{
    auto offset = numTiles * hitCoord->Y;
    auto index = emptyTileCoord.X + numTiles * emptyTileCoord.Y;
    auto nextEmptyTileIndex = hitCoord->X + offset;
    
    if(hitCoord->Y == emptyTileCoord.Y)
    {
        while (index != nextEmptyTileIndex)
        {
            const auto nextIndex = index - movement->X;
            _grid[nextIndex]->MoveTo(movementDir);
            _grid[index] = _grid[nextIndex];
            index -= movement->X;
        }
    }
    else if(hitCoord->X == emptyTileCoord.X)
    {
        while (index != nextEmptyTileIndex)
        {
            const auto nextIndex = index - movement->Y * numTiles;
            _grid[nextIndex]->MoveTo(-movementDir);
            _grid[index] = _grid[nextIndex];
            index -= movement->Y * numTiles;
        }
    }

    emptyTileCoord = *hitCoord;
    _grid[nextEmptyTileIndex] = nullptr;

    // for (int i = 0; i < numTiles * numTiles; i++)
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("tile %d"), _grid[i] == nullptr ? -1 : _grid[i]->GetNum());
    // }
}

static FVector GetTileSpawnPosition(float zPos, int raw, int col)
{
    auto pos = FVector::UpVector * (zPos);
    pos.Y -= tileSize * (initialOffset - col);
    pos.X += tileSize * (initialOffset - raw);
    return pos;
}

void AGameboardActor::SpawnTiles(int num)
{
    UE_LOG(LogTemp, Warning, TEXT("QQQQQQQQQQQQQQQQ %d"), num);

    
    if (_grid != nullptr)
    {
        for (int i = 0; i < lastTileCount; ++i)
        {
            RecycleTile(_grid[i]);
        }

        delete[] _grid;
    }

    lastTileCount = num * num;
    _grid = new ATile*[lastTileCount]{nullptr};
    numTiles = num;

    const auto ratio = static_cast<float>(1) / static_cast<float>(num) * 0.90f;
    tileScale = FVector(ratio, ratio, ratio);
    tileSize = boardWidth / num;
    UE_LOG(LogTemp, Warning, TEXT("tileSize %f %d"), tileSize, num);
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

        const auto tilePos = GetTileSpawnPosition(tileExtents.X, raw, col);
        t->SetActorRelativeLocation(tilePos);
        t->Initialize(i + 1, tileSize, _tileMoveDuration);
        _grid[i] = t;
    }

    emptyTileCoord = Coord(num - 1, num - 1);
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

void AGameboardActor::RecycleTile(ATile* t)
{
    if (t == nullptr)
    {
        return;
    }

    _pool.emplace(t);
    t->SetActive(false);
}

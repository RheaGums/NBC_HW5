// SpawnVolume.cpp


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	
	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);
	
	CurrentWaveIndex = 0;
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	//중심 -> 끝 까지의 거리
	FVector BoxOrigin = SpawningBox->GetComponentLocation();
	
	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X,BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y,BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z,BoxExtent.Z)
		);
}

void ASpawnVolume::SetWaveIndex(int32 NewWaveIndex)
{
	CurrentWaveIndex = NewWaveIndex;
}

FWaveSpawnRow* ASpawnVolume::GetWaveData(int32 WaveIndex) const
{
	if (!LevelDataTable) return nullptr;
	
	TArray<FWaveSpawnRow*> AllRows;
	static const FString ContextString(TEXT("WaveSpawnContext"));
	LevelDataTable->GetAllRows(ContextString,AllRows);
	
	if (AllRows.IsValidIndex(WaveIndex)) return AllRows[WaveIndex];
	return nullptr;
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (SelectedRow->ItemClass)
		{
			return SpawnItem(SelectedRow->ItemClass);
		}
	}
	return nullptr;
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;
	
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ItemClass, GetRandomPointInVolume(),FRotator::ZeroRotator);
	return SpawnedActor;
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	FWaveSpawnRow* WaveData = GetWaveData(CurrentWaveIndex);
	if (!WaveData || !WaveData->ItemTable) return nullptr;
    
	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));
	WaveData->ItemTable->GetAllRows(ContextString, AllRows);
    
	if (AllRows.IsEmpty()) return nullptr;
    
	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row) TotalChance += Row->SpawnChance;
	}
    
	const float RandValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f;
    
	for (FItemSpawnRow* Row : AllRows)
	{
		if (!Row) continue;
       
		AccumulateChance += Row->SpawnChance;
		// 💡 Row->SpawnChance 대신 누적된 확률(AccumulateChance)과 비교해야 합니다!
		if (RandValue <= AccumulateChance) 
		{
			return Row;
		}
	}
    
	// 혹시 모를 예외 처리용으로 마지막 원소 반환
	return AllRows.Last();
}



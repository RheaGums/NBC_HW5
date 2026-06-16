//ItemSpawnRow.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemSpawnRow.generated.h"

USTRUCT(BlueprintType)
struct FItemSpawnRow:public FTableRowBase
{
	GENERATED_BODY()
public:
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ItemClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnChance;
};
USTRUCT(BlueprintType)
struct FWaveSpawnRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaveTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpawnCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* ItemTable;
};
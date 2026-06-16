// SpawnVolume.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnRow.h"
#include "SpawnVolume.generated.h"

struct FItemSpawnRow;
class UBoxComponent;
UCLASS()
class SPARTAPROJECT_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UDataTable* LevelDataTable;
	
	FWaveSpawnRow* GetWaveData(int32 WaveIndex) const;
	
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnRandomItem();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass); //TSubclassOf<AActor> 액터의 하위클래스가 아니면 무조건 오류가 남
	FItemSpawnRow* GetRandomItem() const;
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	FVector GetRandomPointInVolume() const;
	
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SetWaveIndex(int32 NewWaveIndex);
	
private:
	int32 CurrentWaveIndex;

};

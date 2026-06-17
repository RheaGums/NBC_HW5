// SpartaGameState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAPROJECT_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()
public:
	ASpartaGameState();
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	float LevelDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName>LevelMapNames;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Explosion")
	TSubclassOf<class AExplosionActor> ExplosionActorClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWaves;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	float WaveDuration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 ItemSpawnCountPerWave;
	FTimerHandle ExplosionTimerHandle;
	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
	
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore()const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	
	void StartLevel();
	void EndLevel();
	void OnLevelTimeUp();
	
	void StartWave(int32 WaveIndex);
	void EndWave();
	void ClearExistingItems();
	void OnWaveTimeUp();


	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();
	void OnCoinCollected();
	
	void UpdateHUD();
	void ShowWaveNotification(FString Message);
	void StartRandomExplosions();
	void TriggerSingleExplosion();
	void ActivateSpikeTraps();
	
	void ClearNotificationText();
	
};

//SpartaGameState.cpp

#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "ItemSpawnRow.h"
#include "CoinItem.h"
#include "SpikeTrap.h"
#include "ExplosionActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 60.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
	CurrentWaveIndex = 0;
	MaxWaves = 3;
	WaveDuration = 20.0f;
	ItemSpawnCountPerWave = 15;
	
		
}

void ASpartaGameState::BeginPlay()
{
		Super::BeginPlay();
	
		// UpdateHUD();
		StartLevel();
	
		GetWorldTimerManager().SetTimer(
			HUDUpdateTimerHandle,
			this,
			&ASpartaGameState::UpdateHUD,
			0.1f,
			true
		);
}

int32 ASpartaGameState::GetScore() const
{
		return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
			if (SpartaGameInstance)
			{
				SpartaGameInstance->AddToScore(Amount);
			}
		}
}

void ASpartaGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	CurrentWaveIndex = 0;
	
	StartWave(0);
}

void ASpartaGameState::OnLevelTimeUp()
{
	EndLevel();
}

void ASpartaGameState::StartWave(int32 WaveIndex)
{
    CurrentWaveIndex = WaveIndex;
    UE_LOG(LogTemp, Warning, TEXT("Starting Wave %d"), CurrentWaveIndex + 1);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1, 3.0f, FColor::Yellow,
            FString::Printf(TEXT("Starting Wave %d"), CurrentWaveIndex + 1)
        );
    }

    // 💡 첫 웨이브(0번)일 때는 메인 메뉴 전환 직후이므로 0.5초의 안전 시간을 주고,
    // 인게임 내에서 자연스럽게 전환되는 다음 웨이브(1, 2번)들은 기존대로 0.1초만에 바로 실행되도록 가변 딜레이를 적용합니다.
    float InitialDelay = (WaveIndex == 0) ? 0.5f : 0.1f;

    FTimerHandle SpawnDelayHandle;
    GetWorldTimerManager().SetTimer(SpawnDelayHandle, [this, WaveIndex]()
    {
        TArray<AActor*> FoundVolumes;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

        if (FoundVolumes.Num() > 0)
        {
            ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
            if (SpawnVolume)
            {
                FWaveSpawnRow* WaveData = SpawnVolume->GetWaveData(WaveIndex);
                if (WaveData)
                {
                    WaveDuration = WaveData->WaveTime;
                    ItemSpawnCountPerWave = WaveData->SpawnCount;
                }

                SpawnVolume->SetWaveIndex(CurrentWaveIndex);
                
                int32 ActualSpawnedCount = 0; // 로그 확인용 변수
                for (int32 i = 0; i < ItemSpawnCountPerWave; i++)
                {
                    AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
                    if (SpawnedActor)
                    {
                        ActualSpawnedCount++;
                        if (SpawnedActor->IsA(ACoinItem::StaticClass()))
                        {
                            SpawnedCoinCount++;
                        }
                    }
                }
                UE_LOG(LogTemp, Log, TEXT("[GameState] Wave %d 스폰 시도 결과: 요청 %d개 중 %d개 성공"), WaveIndex + 1, ItemSpawnCountPerWave, ActualSpawnedCount);
            }
        }
        else
        {
            // 💡 디버깅용 로그: 만약 0.5초 뒤에도 스폰 볼륨을 못 찾으면 맵 배치 상태를 봐야 합니다.
            UE_LOG(LogTemp, Error, TEXT("[GameState]  월드에서 SpawnVolume 액터를 찾지 못했습니다!"));
        }

        GetWorldTimerManager().SetTimer(
            LevelTimerHandle,
            this,
            &ASpartaGameState::OnWaveTimeUp,
            WaveDuration,
            false
        );

        switch (CurrentWaveIndex)
        {
        case 0:
            ShowWaveNotification(TEXT("Wave 1: 아이템을 수집하세요!"));
            break;
        case 1:
            ShowWaveNotification(TEXT("위험! 스파이크 함정이 발동되었습니다...!"));
            ActivateSpikeTraps();
            break;
        case 2:
            ShowWaveNotification(TEXT("경고! 맵 전역에 무작위 폭발이 발생합니다!"));
            StartRandomExplosions();
        	ActivateSpikeTraps();
            break;
        default:
            break;
        }

    }, InitialDelay, false);
}

void ASpartaGameState::EndWave()
{

	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	
	if (GetWorldTimerManager().IsTimerActive(ExplosionTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(ExplosionTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("웨이브가 종료되어 폭발 타이머를 제거했습니다."));
	}

	ClearExistingItems();

	int32 NextWave = CurrentWaveIndex + 1;
	if (NextWave < MaxWaves)
	{
		StartWave(NextWave);
	}
	else
	{
		EndLevel();
	}
}

void ASpartaGameState::ClearExistingItems()
{

	TArray<AActor*> FoundItems;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseItem::StaticClass(), FoundItems);
    
	for (AActor* Item : FoundItems)
	{
		if (Item)
		{
			Item->Destroy(); // 액터 삭제
		}
	}
	
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
}

void ASpartaGameState::OnWaveTimeUp()
{
	EndWave();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	
	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndWave();
	}
}

void ASpartaGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
			
			if (CurrentLevelIndex >= MaxLevels)
			{
				OnGameOver();
				return;
			}
			
			if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
			{
				FTimerHandle LevelChangeTimer;
				FName NextLevel = LevelMapNames[CurrentLevelIndex];
				GetWorldTimerManager().SetTimer(LevelChangeTimer, [this, NextLevel]()
				{
					UGameplayStatics::OpenLevel(GetWorld(), NextLevel);
				}, 0.1f, false);
			}
			else
			{
				OnGameOver();
			}
		}
	}
}

void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowGameOverMenu();
		}
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
		if (SpartaPlayerController)
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}
				
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}
				
				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
				}
				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"),CurrentWaveIndex+1)));
				}
			}
		}
	}
}
void ASpartaGameState::ShowWaveNotification(FString Message)
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* NotiText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("NotificationText"))))
				{
					NotiText->SetText(FText::FromString(Message));
                    
					FTimerHandle ClearNotiTimer;
					GetWorldTimerManager().SetTimer(
						ClearNotiTimer, 
						this, 
						&ASpartaGameState::ClearNotificationText, 
						3.0f, 
						false
					);
				}
			}
		}
	}
}
void ASpartaGameState::StartRandomExplosions()
{
	GetWorldTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&ASpartaGameState::TriggerSingleExplosion,
		3.0f,
		true // 반복 실행
	);
}

void ASpartaGameState::TriggerSingleExplosion()
{
	if (!ExplosionActorClass) 
	{
		UE_LOG(LogTemp, Error, TEXT("[GameState] ExplosionActorClass가 지정되지 않았습니다!"));
		return;
	}


	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		FVector PlayerLoc = PlayerPawn->GetActorLocation();
		
		FVector ExplosionLoc = PlayerLoc + FVector(FMath::FRandRange(-500.f, 500.f), FMath::FRandRange(-500.f, 500.f), 0.f);
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 어떤 지형이든 씹고 무조건 스폰 보장
        
		GetWorld()->SpawnActor<AExplosionActor>(
			ExplosionActorClass, 
			ExplosionLoc, 
			FRotator::ZeroRotator, 
			SpawnParams
		);
        
		UE_LOG(LogTemp, Warning, TEXT("[GameState] 플레이어 근처 폭발 액터 생성 완료! 위치: %s"), *ExplosionLoc.ToString());
	}
}

void ASpartaGameState::ActivateSpikeTraps()
{
	TArray<AActor*> FoundTraps;
	// 맵에 있는 모든 SpikeTrap 액터를 긁어모읍니다.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpikeTrap::StaticClass(), FoundTraps);

	for (AActor* Actor : FoundTraps)
	{
		if (ASpikeTrap* Trap = Cast<ASpikeTrap>(Actor))
		{
			Trap->SetTrapActive(true);
		}
	}
    
	UE_LOG(LogTemp, Warning, TEXT("맵 안의 모든 스파이크 함정이 활성화되었습니다!"));
}

void ASpartaGameState::ClearNotificationText()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* NotiText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("NotificationText"))))
				{
					NotiText->SetText(FText::FromString(TEXT("")));
				}
			}
		}
	}
}

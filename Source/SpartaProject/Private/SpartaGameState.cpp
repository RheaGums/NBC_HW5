//SpartaGameState.cpp

#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "ItemSpawnRow.h"
#include "CoinItem.h"
#include "SpikeTrap.h"
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
                for (int32 i = 0; i < ItemSpawnCountPerWave; i++)
                {
                    AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
                    if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
                    {
                        SpawnedCoinCount++;
                    }
                }
            }
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
            break;
        default:
            break;
        }

    }, 0.1f, false);
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
	// 필드에 남아있는 모든 아이템을 담을 배열
	TArray<AActor*> FoundItems;
    
	// 💡 [주의] 만약 모든 아이템의 부모 클래스가 ACoinItem이 아니라면, 
	// 프로젝트에서 만든 아이템들의 최상위 부모 C++ 클래스(예: ABaseItem 등)를 넣어주셔야 합니다.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseItem::StaticClass(), FoundItems);
    
	for (AActor* Item : FoundItems)
	{
		if (Item)
		{
			Item->Destroy(); // 액터 삭제
		}
	}

	// 💡 다음 웨이브를 위해 코인 카운트도 초기화해줍니다.
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
				// ✅ 타이머로 한 프레임 뒤에 레벨 전환
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
                    
					// 💡 [수정] 위험한 람다 함수 대신, 객체 안전성이 보장되는 멤버 함수 호출 방식을 씁니다.
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
	// 3초마다 지속적으로 폭발을 일으키는 타이머 가동
	GetWorldTimerManager().SetTimer(
		ExplosionTimerHandle, // 👈 .h 파일에 FTimerHandle ExplosionTimerHandle; 추가 필요
		this,
		&ASpartaGameState::TriggerSingleExplosion,
		3.0f,
		true // 반복 실행
	);
}

void ASpartaGameState::TriggerSingleExplosion()
{
	// 플레이어 캐릭터 위치 가져오기
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		FVector PlayerLoc = PlayerPawn->GetActorLocation();
        
		// 플레이어 주변 무작위 반경 (예: 500 내외) 계산
		FVector ExplosionLoc = PlayerLoc + FVector(FMath::FRandRange(-500.f, 500.f), FMath::FRandRange(-500.f, 500.f), 0.f);
        
		// 1. 여기에 폭발 이펙트(SpawnEmitterAtLocation)나 사운드를 재생하거나
		// 2. 폭발 콜라이더 액터를 해당 위치에 Spawn해서 플레이어가 맞으면 대미지를 입게 처리!
		UE_LOG(LogTemp, Warning, TEXT("플레이어 근처 폭발 발생 위치: %s"), *ExplosionLoc.ToString());
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
			// 💡 함정을 활성화 상태로 만듭니다!
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

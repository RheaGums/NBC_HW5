#include "SpikeTrap.h"

#include "GameplayEffectTypes.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "AbilitySystemComponent.h"

ASpikeTrap::ASpikeTrap()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;

    SpikeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpikeMesh"));
    SpikeMesh->SetupAttachment(RootComponent);


    bIsActive = false;
}

void ASpikeTrap::BeginPlay()
{
    Super::BeginPlay();
    
    // 오버랩 이벤트 바인딩
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASpikeTrap::OnTrapOverlap);
}

void ASpikeTrap::SetTrapActive(bool bNewActive)
{
    bIsActive = bNewActive;
    
    // 블루프린트에서 외형적 변화(예: 슥 솟아오르기, 빨간 불 켜기 등)를 주기 쉽게 로그나 이벤트를 연동할 수 있습니다.
    UE_LOG(LogTemp, Warning, TEXT("%s 함정 활성화 상태 변경: %s"), *GetName(), bIsActive ? TEXT("True") : TEXT("False"));
}

void ASpikeTrap::OnTrapOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                               bool bFromSweep, const FHitResult& SweepResult)
{
    // 함정이 활성화(Wave 2) 상태이고 부딪힌 액터가 있을 때
    if (bIsActive && OtherActor)
    {
        // 부딪힌 액터가 플레이어인지 확인 (태그 검사)
        if (OtherActor->ActorHasTag(TEXT("Player")))
        {
            UE_LOG(LogTemp, Warning, TEXT("[SpikeTrap] 💥 플레이어 가시 밟음! 기본 대미지를 적용합니다."));

            // 지뢰와 동일한 언리얼 기본 대미지 시스템 사용
            UGameplayStatics::ApplyDamage(
                OtherActor,
                10.0f,
                nullptr,
                this,
                UDamageType::StaticClass()
            );
        }
    }
}



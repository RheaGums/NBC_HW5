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
    

    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASpikeTrap::OnTrapOverlap);
}

void ASpikeTrap::SetTrapActive(bool bNewActive)
{
    bIsActive = bNewActive;
    
    UE_LOG(LogTemp, Warning, TEXT("%s 함정 활성화 상태 변경: %s"), *GetName(), bIsActive ? TEXT("True") : TEXT("False"));
}

void ASpikeTrap::OnTrapOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                               bool bFromSweep, const FHitResult& SweepResult)
{

    if (bIsActive && OtherActor)
    {

        if (OtherActor->ActorHasTag(TEXT("Player")))
        {
            UE_LOG(LogTemp, Warning, TEXT("[SpikeTrap] 플레이어 가시 밟음! 기본 대미지를 적용합니다."));


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



// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosionActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

AExplosionActor::AExplosionActor()
{
	ItemType = FName("Explosion");
	ExplosionDamage = 20.0f;

	if (Collision)
	{
		Collision->InitSphereRadius(250.0f);
	}
}

void AExplosionActor::BeginPlay()
{
	Super::BeginPlay();
    
	TriggerExplosion();
}


void AExplosionActor::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator); 

	if (Activator && Activator->ActorHasTag(TEXT("Player")))
	{
		UGameplayStatics::ApplyDamage(Activator, ExplosionDamage, nullptr, this, UDamageType::StaticClass());
		UE_LOG(LogTemp, Warning, TEXT("[Explosion] 플레이어가 폭발 영역에 진입해 대미지를 입었습니다!"));
	}

	SetLifeSpan(0.1f);
}

void AExplosionActor::TriggerExplosion()
{

	if (PickupParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupParticle, GetActorLocation(), GetActorRotation());
	}
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
	}


	TArray<AActor*> OverlappingActors;
	Collision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->IsValidLowLevel() && Actor->ActorHasTag(TEXT("Player")))
		{
			UGameplayStatics::ApplyDamage(Actor, ExplosionDamage, nullptr, this, UDamageType::StaticClass());
			UE_LOG(LogTemp, Warning, TEXT("[Explosion] 무작위 폭발 범위 대미지 발동!"));
		}
	}

	SetLifeSpan(0.1f);
}


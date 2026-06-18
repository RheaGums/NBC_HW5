// ExplosionActor.cpp


#include "ExplosionActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/SphereComponent.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"

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
		IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Activator);
		if (!ASCInterface || !DamageEffectClass) return;

		UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
		if (!ASC) return;

		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
			DamageEffectClass,
			1.0f,
			EffectContext);

		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(
				FGameplayTag::RequestGameplayTag(FName("Damage.Amount")),
				-ExplosionDamage);

			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
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
			IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Actor);
			if (!ASCInterface || !DamageEffectClass) continue;

			UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
			if (!ASC) continue;

			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
				DamageEffectClass,
				1.0f,
				EffectContext);

			if (SpecHandle.IsValid())
			{
				SpecHandle.Data->SetSetByCallerMagnitude(
					FGameplayTag::RequestGameplayTag(FName("Damage.Amount")),
					-ExplosionDamage);

				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
			UE_LOG(LogTemp, Warning, TEXT("[Explosion] 무작위 폭발 범위 대미지 발동!"));
		}
	}

	SetLifeSpan(0.1f);
}


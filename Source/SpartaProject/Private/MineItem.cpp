//MineItem.cpp

#include "MineItem.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/SphereComponent.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"
#include  "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
{
	ExplosionDelay = 5.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 30.0f;
	ItemType = "Mine";
	bHasExploded = false;
    
	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->InitSphereRadius(ExplosionRadius);
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision->SetupAttachment(Scene);
	
	
}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (Activator == nullptr) return;
	if (bHasExploded) return;
	Super::ActivateItem(Activator);
	// 5초 후 폭발 실행
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AMineItem::Explode, ExplosionDelay,false);
	bHasExploded = true;
}

void AMineItem::Explode()
{
	UParticleSystemComponent* Particle = nullptr;
	if (ExplosionParticle)
	{
		 Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle, 
			GetActorLocation(), 
			GetActorRotation(),
			false);
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation());
	}
	
	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->IsValidLowLevel() && Actor->ActorHasTag("Player"))
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
		}
	}

	// 지뢰 제거
	DestroyItem();
	
	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DestroyParticleTimerHandle,
			[Particle]()
			{
				if (Particle && Particle->IsValidLowLevel())
				{
					Particle->DestroyComponent();
				}
			},
			2.0f,
			false);
	}
}

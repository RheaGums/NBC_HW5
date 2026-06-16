// Fill out your copyright notice in the Description page of Project Settings.


#include "DebuffItem.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

ADebuffItem::ADebuffItem()
{
	ItemType = FName("Debuff");
}

void ADebuffItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	if (!Activator || !DebuffEffectClass) return;
	
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Activator);
	if (!ASCInterface) return;
	
	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (!ASC) return;
	
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
		DebuffEffectClass,
		1.0f,
		EffectContext);
	
	if (SpecHandle.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
	
	SetLifeSpan(0.1f);
}

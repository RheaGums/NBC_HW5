// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ExplosionActor.generated.h"

class UGameplayEffect;

/**
 * 
 */
UCLASS()
class SPARTAPROJECT_API AExplosionActor : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AExplosionActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion|Settings")
	float ExplosionDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS | Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	virtual void ActivateItem(AActor* Activator) override;
    
	void TriggerExplosion();
};

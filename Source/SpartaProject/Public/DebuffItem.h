// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "GameplayEffect.h"
#include "DebuffItem.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAPROJECT_API ADebuffItem : public ABaseItem
{
	GENERATED_BODY()
public:
	ADebuffItem();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debuff")
	TSubclassOf<UGameplayEffect> DebuffEffectClass;
	
	virtual void ActivateItem(AActor* Activator) override;
};

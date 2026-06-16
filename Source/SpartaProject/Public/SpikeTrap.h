// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpikeTrap.generated.h"

class UBoxComponent;

UCLASS()
class SPARTAPROJECT_API ASpikeTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpikeTrap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CollisionBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SpikeMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap Settings")
	bool bIsActive;
	
	
	UFUNCTION()
	void OnTrapOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
					  bool bFromSweep, const FHitResult& SweepResult);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS | Trap")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;
public:	

	UFUNCTION(BlueprintCallable, Category = "Trap")
	void SetTrapActive(bool bNewActive);

};

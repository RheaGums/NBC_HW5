#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "SpartaAttributeSet.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "SpartaCharacter.generated.h"

class UWidgetComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class SPARTAPROJECT_API ASpartaCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ASpartaCharacter();
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* SpringArmComp;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComp;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    UWidgetComponent* OverheadWidget;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    UAbilitySystemComponent* AbilitySystemComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    USpartaAttributeSet* AttributeSet;
    
    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealth() const;
    
    UFUNCTION(BlueprintCallable, Category = "Health")
    void AddHealth(float Amount);
    
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    

protected:
    float SprintSpeedMultiplier;
    UFUNCTION(BlueprintCallable, Category = "Health")
    virtual void OnDeath();
    void UpdateOverheadHP();
    void OnMoveSpeedChanged(const FOnAttributeChangeData& Data);
    void OnBlindTagChanged(const FGameplayTag Tag, int32 Count);
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> BlindWidgetClass;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UUserWidget* BlindWidgetInstance;
    
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(
       float DamageAmount,
       struct FDamageEvent const& DamageEvent,
       class AController* EventInstigator,
       AActor* DamageCauser
       ) override;

    
    UFUNCTION()
    void Move(const FInputActionValue& value);
    UFUNCTION()
    void StartJump(const FInputActionValue& value);
    UFUNCTION()
    void StopJump(const FInputActionValue& value);
    UFUNCTION()
    void Look(const FInputActionValue& value);
    UFUNCTION()
    void StartSprint(const FInputActionValue& value);
    UFUNCTION()
    void StopSprint(const FInputActionValue& value);
    
    void BeginPlay() override;
};
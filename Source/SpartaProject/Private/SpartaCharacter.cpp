// SpartaCharacter.cpp

#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "EnhancedInputComponent.h"
#include "SpartaGameState.h"
#include "Camera/CameraComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"

ASpartaCharacter::ASpartaCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 300.0f;
    SpringArmComp->bUsePawnControlRotation = true;
    SpringArmComp->bInheritYaw = true;
    SpringArmComp->bInheritPitch = true;
    SpringArmComp->bInheritRoll = false;
    
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;
    
    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
    OverheadWidget->SetupAttachment(GetMesh());
    OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
    
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
    AttributeSet = CreateDefaultSubobject<USpartaAttributeSet>(TEXT("AttributeSet"));

    // 카메라 방향으로 캐릭터 회전 끄기
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    // 이동 방향으로 자동 회전
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

    SprintSpeedMultiplier = 1.7f;
    
}

float ASpartaCharacter::GetHealth() const
{
    if (AttributeSet)
    {
        return AttributeSet->GetHealth();
    }
    return 0.0f;
}

void ASpartaCharacter::AddHealth(float Amount)
{
    if (AttributeSet)
    {
        float NewHealth = FMath::Clamp(AttributeSet->GetHealth()+Amount,0.0f,AttributeSet->GetMaxHealth());
        AttributeSet->SetHealth(NewHealth);
        UpdateOverheadHP();
    }
}

UAbilitySystemComponent* ASpartaCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

float ASpartaCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                   class AController* EventInstigator, AActor* DamageCauser)
{
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ASpartaCharacter::OnDeath()
{
    ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
    if (SpartaGameState)
    {
        SpartaGameState->OnGameOver();
    }
}

void ASpartaCharacter::UpdateOverheadHP()
{
    if (!OverheadWidget) return;
    
    UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
    if (!OverheadWidgetInstance) return;
    
    if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
    {
        HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"),  AttributeSet->GetHealth(),
    AttributeSet->GetMaxHealth())));
    }
}

void ASpartaCharacter::OnMoveSpeedChanged(const FOnAttributeChangeData& Data)
{
    UE_LOG(LogTemp, Warning, TEXT("OldValue: %f, NewValue: %f"), Data.OldValue, Data.NewValue);
    GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void ASpartaCharacter::OnBlindTagChanged(const FGameplayTag Tag, int32 Count)
{
    if (Count>0)
    {
        if (BlindWidgetClass&&!BlindWidgetInstance)
        {
            BlindWidgetInstance = CreateWidget<UUserWidget>(GetWorld(),BlindWidgetClass);
            
            if (!IsValid(this)) return;
            
            if (BlindWidgetInstance)
            {
                BlindWidgetInstance->AddToViewport();
                FObjectProperty* Prop = FindFProperty<FObjectProperty>(BlindWidgetInstance->GetClass(), FName("BlindIn"));
                if (Prop)
                {
                    UWidgetAnimation* Anim = Cast<UWidgetAnimation>(Prop->GetObjectPropertyValue_InContainer(BlindWidgetInstance));
                    if (Anim)
                    {
                        BlindWidgetInstance->PlayAnimation(Anim);
                    }
                }
            }
        }
    }
    else
    {
        if (BlindWidgetInstance)
        {
            FObjectProperty* Prop = FindFProperty<FObjectProperty>(BlindWidgetInstance->GetClass(), FName("BlindOut"));
            if (Prop)
            {
                UWidgetAnimation* Anim = Cast<UWidgetAnimation>(Prop->GetObjectPropertyValue_InContainer(BlindWidgetInstance));
                if (Anim)
                {
                    BlindWidgetInstance->PlayAnimation(Anim);
                    FTimerHandle TimerHandle;
                    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
                    {
                        if (BlindWidgetInstance)
                        {
                            BlindWidgetInstance->RemoveFromParent();
                            BlindWidgetInstance = nullptr;
                        }
                    }, Anim->GetEndTime(), false);
                
                    return;
                }
            }
            BlindWidgetInstance->RemoveFromParent();
            BlindWidgetInstance = nullptr;
        }
    }
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
        {
            if (PlayerController->MoveAction)
            {
                EnhancedInput->BindAction(
                    PlayerController->MoveAction,
                    ETriggerEvent::Triggered, this,
                    &ASpartaCharacter::Move);
            }
            if (PlayerController->JumpAction)
            {
                EnhancedInput->BindAction(
                    PlayerController->JumpAction,
                    ETriggerEvent::Triggered, this,
                    &ASpartaCharacter::StartJump);
                EnhancedInput->BindAction(
                    PlayerController->JumpAction,
                    ETriggerEvent::Completed, this,
                    &ASpartaCharacter::StopJump);
            }
            if (PlayerController->LookAction)
            {
                EnhancedInput->BindAction(
                    PlayerController->LookAction,
                    ETriggerEvent::Triggered, this,
                    &ASpartaCharacter::Look);
            }
            if (PlayerController->SprintAction)
            {
                EnhancedInput->BindAction(
                    PlayerController->SprintAction,
                    ETriggerEvent::Started, this,
                    &ASpartaCharacter::StartSprint);
                EnhancedInput->BindAction(
                    PlayerController->SprintAction,
                    ETriggerEvent::Completed, this,
                    &ASpartaCharacter::StopSprint);
            }
        }
    }
}

void ASpartaCharacter::Move(const FInputActionValue& value)
{
    if (!Controller) return;
    
    const FVector2D MoveInput = value.Get<FVector2D>();
    
    // 카메라 기준 방향 가져오기
    const FRotator ControlRotation = Controller->GetControlRotation();
    const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
    
    if (!FMath::IsNearlyZero(MoveInput.X))
    {
        const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(ForwardDir, MoveInput.X);
    }
    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
        const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(RightDir, MoveInput.Y);
    }
}

void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
    if (value.Get<bool>())
    {
        Jump();
    }
}

void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
    if (!value.Get<bool>())
    {
        StopJumping();
    }
}

void ASpartaCharacter::Look(const FInputActionValue& value)
{
    FVector2D LookInput = value.Get<FVector2D>();
    
    AddControllerYawInput(LookInput.X);
    AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& Value)
{
    if (GetCharacterMovement() && AttributeSet)
    {
        GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetMoveSpeed() * SprintSpeedMultiplier;
    }
}

void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
    if (GetCharacterMovement() && AttributeSet)
    {
        GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetMoveSpeed();
    }
}

void ASpartaCharacter::BeginPlay()
{
    Super::BeginPlay();
    UpdateOverheadHP();
    
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
        
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(USpartaAttributeSet::GetMoveSpeedAttribute()
            ).AddUObject(this, &ASpartaCharacter::OnMoveSpeedChanged);
        
        FGameplayTag BlindTag = FGameplayTag::RequestGameplayTag(FName("Debuff.Blind"));
        AbilitySystemComponent->RegisterGameplayTagEvent(BlindTag,EGameplayTagEventType::NewOrRemoved)
        .AddUObject(this, &ASpartaCharacter::OnBlindTagChanged);

    }
    if (AttributeSet)
    {
        GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetMoveSpeed();
    }
}
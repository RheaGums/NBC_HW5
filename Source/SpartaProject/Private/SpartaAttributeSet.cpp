// SpartaAttributeSet.cpp


#include "SpartaAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "SpartaCharacter.h"

USpartaAttributeSet::USpartaAttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);

	InitMoveSpeed(600.f);
}

void USpartaAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

		// 오너 캐릭터 가져오기
		ASpartaCharacter* OwnerCharacter = Cast<ASpartaCharacter>(GetOwningActor());
		if (OwnerCharacter)
		{
			OwnerCharacter->UpdateOverheadHP();
            
			if (GetHealth() <= 0.0f)
			{
				OwnerCharacter->OnDeath();
			}
		}
	}
}

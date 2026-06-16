// Item.h

#pragma once

#include "CoreMinimal.h"  
#include "GameFramework/Actor.h"
#include "Item.generated.h" //���÷��� �ý��۰� ����



UCLASS() //���÷��� �ý��۰� ����
class SPARTAPROJECT_API AItem : public AActor
	// Actor - A
	// Object - U
	// ����ü - F
	// Enum - E
{
	GENERATED_BODY() //���÷��� �ý��۰� ����
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Components")
	USceneComponent* SceneRoot;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Components")
	UStaticMeshComponent* StaticMeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Properties")
	float RotationSpeed;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "Item|Actions")
	void ResetActorPosition();
	
	
	UFUNCTION(BlueprintPure, Category = "Item|Properties")
	float GetRotationSpeed() const;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Item|Event")
	void OnItemPickedUP();


};

// ������ - �޸𸮿� ����. �� �ѹ� ȣ��.
// PostInitializeComponents() - ������Ʈ�� �ϼ��� ���� ȣ��. ������Ʈ ���� ������ �ְ�ޱ�, ��ȣ�ۿ�.
// BeginPlay() - ��ġ (Spawn) ���� 
// Tick(float DeltaTime) - �� �����Ӹ��� ȣ���.
// Destroyed() - ���� �Ǳ� ������ ȣ���.
// EndPlay() - ���� ����, �ı� (Destroyed()), ���� ��ȯ.
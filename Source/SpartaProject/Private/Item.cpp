// Item.cpp


#include "Item.h"


// Sets default values
AItem::AItem()
{	
	// Scene Component�� �����ϰ� ��Ʈ�� ����
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// Static Mesh Component�� �����ϰ� Scene Component�� Attach
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneRoot);

	PrimaryActorTick.bCanEverTick = true;
	RotationSpeed = 90.0f;
}



void AItem::BeginPlay()
{
	Super::BeginPlay();
	OnItemPickedUP();
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FMath::IsNearlyZero(RotationSpeed)) {
		AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
	}

	
}

void AItem::ResetActorPosition()
{
	SetActorLocation(FVector::ZeroVector);
}

float AItem::GetRotationSpeed() const
{
	return RotationSpeed;
}


// ������ - �޸𸮿� ����. �� �ѹ� ȣ��.
// PostInitializeComponents() - ������Ʈ�� �ϼ��� ���� ȣ��. ������Ʈ ���� ������ �ְ�ޱ�, ��ȣ�ۿ�.
// BeginPlay() - ��ġ (Spawn) ���� 
// Tick(float DeltaTime) - �� �����Ӹ��� ȣ���.
// Destroyed() - ���� �Ǳ� ������ ȣ���.
// EndPlay() - ���� ����, �ı� (Destroyed()), ���� ��ȯ.



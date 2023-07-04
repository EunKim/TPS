// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// Ÿ�� ã��
	target = Cast<ATPSPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass()));
	//�� (������Ʈ�� ������ �ִ� �ڽ�)
	me = Cast<AEnemy>(GetOwner());
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	// FSM ����
	switch (mState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}
}

void UEnemyFSM::IdleState()
{
	//�����ð��� ������ ���¸� �̵����� �ٲٰ� �ʹ�.
	//1. ���¸� �̵����� �ٲٰ� �ʹ�.
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2. ����ð��� ���ð��� �ʰ������ϱ�
	if (currentTime > idleDelayTime)
	{
		//3. ���¸� �̵����� �ٲٰ� �ʹ�.
		mState = EEnemyState::Move;
		currentTime = 0;
	}

}

//Ÿ�������� �̵��ϰ� �ʹ�.
//�ʿ�Ӽ� : Ÿ��, �̵�
void UEnemyFSM::MoveState()
{
	//���� �����ϰ� �ִ� ����
	//1. ������ �ʿ��ϴ�.
	FVector direction = target->GetActorLocation() - me->GetActorLocation();
	direction.Normalize();
	
	//2. P = P0 + vt
	/*FVector P = me->GetActorLocation() + direction * speed * GetWorld()->DeltaTimeSeconds;
	//3. �̵��ϰ� �ʹ�.
	me->SetActorLocation(P);*/
	me->AddMovementInput(direction); 


	/*/direction.Z = 0;
	//Ÿ�ٹ������� ȸ���ϱ�
	// forward vector �� direction�� �ǰ� �ʹ�.
	// ������ �ٲ�� �ϳ� ���ڱ� ������ �ٲ�µ��ϰ� �ٲ�
	FVector forward = me->GetActorForwardVector();
	//�ε巴�� ȸ���Ͽ� Ÿ�ٹ������� �ٲ��� ���ؼ� ( forward �� direction ���� ���������� 1(100%)�� �����ǵ��ΰ� �Ѵ�)
	//forward =FMath::Lerp<FVector,float>(forward,direction, 1);
	forward =FMath::Lerp(forward,direction, 5 * GetWorld()->DeltaTimeSeconds);
	 
	//Enemy foward ���Ͱ� direction �������� ��ġ��Ű�� �ʹ�.
	me->SetActorRotation(direction.Rotation());*/
}

void UEnemyFSM::AttackState()
{

}

void UEnemyFSM::DamageState()
{

}

void UEnemyFSM::DieState()
{

}


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

	// 타켓 찾기
	target = Cast<ATPSPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass()));
	//나 (컴포넌트를 가지고 있는 자신)
	me = Cast<AEnemy>(GetOwner());
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	// FSM 목차
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
	//일정시간이 지나면 상태를 이동으로 바꾸고 싶다.
	//1. 상태를 이동으로 바꾸고 싶다.
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2. 경과시간이 대기시간을 초과했으니깐
	if (currentTime > idleDelayTime)
	{
		//3. 상태를 이동으로 바꾸고 싶다.
		mState = EEnemyState::Move;
		currentTime = 0;
	}

}

//타겟쪽으로 이동하고 싶다.
//필요속성 : 타겟, 이동
void UEnemyFSM::MoveState()
{
	//나를 소유하고 있는 액터
	//1. 방향이 필요하다.
	FVector direction = target->GetActorLocation() - me->GetActorLocation();
	direction.Normalize();
	
	//2. P = P0 + vt
	/*FVector P = me->GetActorLocation() + direction * speed * GetWorld()->DeltaTimeSeconds;
	//3. 이동하고 싶다.
	me->SetActorLocation(P);*/
	me->AddMovementInput(direction);

	/*/direction.Z = 0;
	//타겟방향으로 회전하기
	// forward vector 가 direction이 되고 싶다.
	// 방향이 바뀌긴 하나 갑자기 방향이 바뀌는듯하게 바뀜
	FVector forward = me->GetActorForwardVector();
	//부드럽게 회전하여 타겟방향으로 바뀌기기 위해서 ( forward 가 direction 으로 점차적으로 1(100%)로 보관되도로고 한다)
	//forward =FMath::Lerp<FVector,float>(forward,direction, 1);
	forward =FMath::Lerp(forward,direction, 5 * GetWorld()->DeltaTimeSeconds);
	 
	//Enemy foward 벡터가 direction 방향으로 일치시키고 싶다.
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


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"


//enemy의 상태를 나타낼 enum 자료형 만들기
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle UMETA(DisplayName="Idle State"),
	Move,
	Attack,
	Damage,
	Die

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FSM")
	EEnemyState mState = EEnemyState::Idle;

	// 필요속성 : 대기시간, 경과시간
	UPROPERTY(EditAnywhere, Category="FSM")
	float idleDelayTime = 2;
	float currentTime = 0;

	//필요속성 : 타겟, 이동속도
	UPROPERTY(EditDefaultsOnly)
	class ATPSPlayer* target;
	
	UPROPERTY(EditAnywhere, Category="FSM")
	float speed = 500;
	
	//나를 소유하고 있는 액터
	UPROPERTY(EditDefaultsOnly)
	class AEnemy* me;


	//상태 함수들
	void IdleState();
	void MoveState();
	void AttackState();
	void DamageState();
	void DieState();
};

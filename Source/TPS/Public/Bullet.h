// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"



UCLASS()
class TPS_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Die();

	//총알이 바라보는 방향으로 이동하고 싶다.

	UPROPERTY(EditAnywhere)
	class USphereComponent* CollisionComp;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* BodyMeshComp;

	UPROPERTY(EditAnywhere)
	class UProjectileMovementComponent* ProjectileComp;



};

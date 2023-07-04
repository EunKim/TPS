// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;
	CollisionComp->SetCollisionProfileName(TEXT("BlockAll"));
	CollisionComp->SetSphereRadius(12.5f);

	BodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));
	BodyMeshComp->SetupAttachment(RootComponent);
	BodyMeshComp->SetRelativeScale3D(FVector(0.25f));

	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileComp->InitialSpeed = 5000;
	ProjectileComp->MaxSpeed = 5000;
	ProjectileComp->bShouldBounce = true;
	ProjectileComp->Bounciness = 0.5f;
	ProjectileComp->SetUpdatedComponent(CollisionComp);

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempComp(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));

	if (TempComp.Succeeded())
	{
		BodyMeshComp->SetStaticMesh(TempComp.Object);
	}

	ConstructorHelpers::FObjectFinder<UMaterial> TempMat(TEXT("/Script/Engine.Material'/DatasmithContent/Materials/FBXImporter/DeltaGenReference.DeltaGenReference'"));

	if (TempMat.Succeeded())
	{
		BodyMeshComp->SetMaterial(0,TempMat.Object);
		
	}
	
	//SetLifeSpan(2);

	
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	// 죽을 날짜 받아놓자..
	FTimerHandle DieTimer;
	GetWorldTimerManager().SetTimer(DieTimer, this, &ABullet::Die, 2.0f, false);
}
// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::Die()
{
	Destroy();
	
}




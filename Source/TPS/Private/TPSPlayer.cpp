// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include "Engine/StaticMesh.h"
#include "Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Engine/Classes/Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Bullet.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//Mesh 데이터 할당
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));

	if (TempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-90), FRotator(0,-90,0));
	}

	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0, 70, 60));

	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("tpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);

	bUseControllerRotationYaw = true;
	springArmComp->bUsePawnControlRotation = true;
	tpsCamComp->bUsePawnControlRotation = false;

	//유탄총 추가
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("gunMeshComp"));
	gunMeshComp->SetupAttachment(GetMesh());

	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("sniperGunComp"));
	sniperGunComp->SetupAttachment(GetMesh());
	

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (TempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		gunMeshComp->SetRelativeLocation(FVector(-20, 30, 120));
	}

	//총알공장 ABullet 타입의 블루프린트 가져와서 할당하기.
	ConstructorHelpers::FClassFinder<ABullet> TempBullet(TEXT("/Script/Engine.Blueprint'/Game/BluePrints/BP_Bullet.BP_Bullet_C'"));

	if (TempBullet.Succeeded())
	{
		bulletFactory = TempBullet.Class;
	}


	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("/Script/Engine.StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	if (TempSniperMesh.Succeeded())
	{
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		sniperGunComp->SetRelativeLocation(FVector(-30, 60, 110));
		sniperGunComp->SetRelativeScale3D(FVector(0.205f));
		//sniperGunComp->SetRelativeRotation(FRotator(0.205f));
	}



}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	gunMeshComp->SetVisibility(false);
	sniperGunComp->SetVisibility(true);

	//crosshair ui 인스턴스 만들기
	crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);
	if (crosshairUI != nullptr)
	{
		crosshairUI->AddToViewport();
	}

}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::Horizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::Vertical);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATPSPlayer::Turn);
	PlayerInputComponent->BindAxis(TEXT("Lookup"), this, &ATPSPlayer::Lookup);

	PlayerInputComponent->BindAction(TEXT("Fire"),  IE_Pressed, this, &ATPSPlayer::Fire);
	PlayerInputComponent->BindAction(TEXT("GrenadeGun"),  IE_Pressed, this, &ATPSPlayer::GrenadeGun);
	PlayerInputComponent->BindAction(TEXT("SniperGun"),  IE_Pressed, this, &ATPSPlayer::SniperGun);



}

void ATPSPlayer::Horizontal(float value)
{
	AddMovementInput(GetActorRightVector(),value);

}

void ATPSPlayer::Vertical(float value)
{
	AddMovementInput(GetActorForwardVector(),value);
}

void ATPSPlayer::Turn(float value)
{
	AddControllerYawInput(value);
}

void ATPSPlayer::Lookup(float value)
{
	AddControllerPitchInput(value);
}

void ATPSPlayer::Fire()
{
	
	//유탄총 사용중일때 유탄발사
	if (bUseGrenadeGun) {
		// 총알 발사하고 싶다.
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}

	// 그렇지 않으면 스나이퍼건 발사
	else {
		
		//LineTrace 를 쏴서 부딪힌, 지점에 파편효과 발생시키고 싶다.
		//1. 시작점이 필요하다. (월드 기준의 좌표가 필요)
		FVector startPos = tpsCamComp->GetComponentLocation();
		//2. 종료점이 필요하다. (눈에서 부터 떨어서진 50m(5000) 방향)
		FVector endPos = startPos + tpsCamComp->GetForwardVector() * 5000;
		//3. 선을 만들어야 한다.
		FHitResult hitinfo;
		//-> 나는 안맞게 하고싶다.
		FCollisionQueryParams param;
		param.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(hitinfo,startPos,endPos, ECC_Visibility ,param);
		//4. 선이 부딪혔으니깐
		if (bHit)
		{
			//5. 부딪힌 물체 날려보내기
			// 부딪힌 녀석의 컴포넌트가 SimulatePhysics가 활성화 되어있어야 한다.
			//->물리 활성화 되어 있는지 체크하고 싶다.
			auto hitComp = hitinfo.GetComponent();
			//->부딪힌 녀석의 컴포넌트가 필요하다
			if (hitComp->IsSimulatingPhysics())
			{
				//그녀석 날려보내자.
				//P = P0 + vt, v = v0 + at
				// F = ma (컴포넌트질량(mass) * 카메라가 보는방향으로(a) * 속도(10000))
				FVector force = hitComp->GetMass() * tpsCamComp->GetForwardVector() * 100000;
				//FVector force = hitComp->GetMass() * tpsCamComp->GetForwardVector() * 5000;
				//AddForceAtLocation 맞은 기준점 기준으로 날라감(ex. 머리를 맞았다고 하면 머리가 팅겨져 나가는기능처럼)
				hitComp->AddForceAtLocation(force,hitinfo.ImpactPoint);
			}
			//6. 파편효과 발생
			FTransform trans;
			trans.SetLocation(hitinfo.ImpactPoint);
			trans.SetRotation(hitinfo.ImpactNormal.ToOrientationQuat());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, trans);
		}
		

	}


}


void ATPSPlayer::GrenadeGun()
{
	bUseGrenadeGun = true;
	gunMeshComp->SetVisibility(true);
	sniperGunComp->SetVisibility(false);

}

void ATPSPlayer::SniperGun()
{
	bUseGrenadeGun = false;
	gunMeshComp->SetVisibility(false);
	sniperGunComp->SetVisibility(true);
	
	
}


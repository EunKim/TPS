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


	//Mesh ������ �Ҵ�
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

	//��ź�� �߰�
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

	//�Ѿ˰��� ABullet Ÿ���� �������Ʈ �����ͼ� �Ҵ��ϱ�.
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

	//crosshair ui �ν��Ͻ� �����
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
	
	//��ź�� ������϶� ��ź�߻�
	if (bUseGrenadeGun) {
		// �Ѿ� �߻��ϰ� �ʹ�.
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}

	// �׷��� ������ �������۰� �߻�
	else {
		
		//LineTrace �� ���� �ε���, ������ ����ȿ�� �߻���Ű�� �ʹ�.
		//1. �������� �ʿ��ϴ�. (���� ������ ��ǥ�� �ʿ�)
		FVector startPos = tpsCamComp->GetComponentLocation();
		//2. �������� �ʿ��ϴ�. (������ ���� ����� 50m(5000) ����)
		FVector endPos = startPos + tpsCamComp->GetForwardVector() * 5000;
		//3. ���� ������ �Ѵ�.
		FHitResult hitinfo;
		//-> ���� �ȸ°� �ϰ�ʹ�.
		FCollisionQueryParams param;
		param.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(hitinfo,startPos,endPos, ECC_Visibility ,param);
		//4. ���� �ε������ϱ�
		if (bHit)
		{
			//5. �ε��� ��ü ����������
			// �ε��� �༮�� ������Ʈ�� SimulatePhysics�� Ȱ��ȭ �Ǿ��־�� �Ѵ�.
			//->���� Ȱ��ȭ �Ǿ� �ִ��� üũ�ϰ� �ʹ�.
			auto hitComp = hitinfo.GetComponent();
			//->�ε��� �༮�� ������Ʈ�� �ʿ��ϴ�
			if (hitComp->IsSimulatingPhysics())
			{
				//�׳༮ ����������.
				//P = P0 + vt, v = v0 + at
				// F = ma (������Ʈ����(mass) * ī�޶� ���¹�������(a) * �ӵ�(10000))
				FVector force = hitComp->GetMass() * tpsCamComp->GetForwardVector() * 100000;
				//FVector force = hitComp->GetMass() * tpsCamComp->GetForwardVector() * 5000;
				//AddForceAtLocation ���� ������ �������� ����(ex. �Ӹ��� �¾Ҵٰ� �ϸ� �Ӹ��� �ð��� �����±��ó��)
				hitComp->AddForceAtLocation(force,hitinfo.ImpactPoint);
			}
			//6. ����ȿ�� �߻�
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


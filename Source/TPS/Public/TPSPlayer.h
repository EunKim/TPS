// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "TPSPlayer.generated.h"

UCLASS()
class TPS_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//�ʿ��� ������Ʈ ����
    UPROPERTY(VisibleAnywhere)
    class USpringArmComponent* springArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UCameraComponent* tpsCamComp;

	//��ź �� ������Ʈ ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* gunMeshComp;

	//��ź �� ������Ʈ ����
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UStaticMeshComponent* sniperGunComp;

	//����� �Է�ó�� �Լ�
	
	// �¿� �̵�
	void Horizontal(float value);
	void Vertical(float value);

	//���콺�� ���� ȸ��
	void Turn(float value);
	void Lookup(float value);

	//�ÿ��ڰ� �߻��ư�� ������ �Ѿ��� �߻��ϰ� �ʹ�.
	// �ʿ�Ӽ� : �Ѿ˰���

	void Fire();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABullet> bulletFactory;

	//1,2���� ���� �� �ٲٱ�
	void GrenadeGun();
	void SniperGun();

	//���� ��ź�߻����� ��������� ��� : flag
	bool bUseGrenadeGun = false;

	UPROPERTY(EditDefaultsOnly, Category="Effect")
	class UParticleSystem* bulletEffectFactory;

	//corsshari ui ����
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class UUserWidget> crosshairUIFactory;

	//crosshair factory�� ���� ������� �ν��Ͻ�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	class UUserWidget* crosshairUI;
};

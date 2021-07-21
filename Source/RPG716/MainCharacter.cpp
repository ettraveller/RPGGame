// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Math/RotationMatrix.h>
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create CameraBoom( pull toward thee player if there's a collison)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	GetCapsuleComponent()->SetCapsuleSize(48.f, 88.f);
		
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);//socket = 에셋을 넣을수있는 장소
	FollowCamera->bUsePawnControlRotation = false;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	//카메라와 캐릭터 로테이션 분리
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;

	//캐릭터가 자동으로 이동방향에 따라 방향설정
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 840.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.3f;

	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 350.f;
	Stamina = 120.f;
	Coins = 0;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &AMainCharacter::StopJumping);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AMainCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);


}

void AMainCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.0)
	{
	    const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.0)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::TurnAtRate(float Rate)
{
   AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


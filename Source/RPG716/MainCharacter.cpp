// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Math/RotationMatrix.h>
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include <Animation/AnimMontage.h>
#include "Sound/SoundCue.h"
#include <Kismet/GameplayStatics.h>

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
	MaxStamina = 150.f;
	Stamina = 120.f;
	Coins = 0;

	RunningSpeed = 600.f;
	SprintSpeed = 1000.f;

	bShiftKeyDown = false;
	bLMBDown = false;

	// Initialize Enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;

}

void AMainCharacter::ShowPickUpLocation()
{
// tarray for문 돌리는거임
	/*for (int32 i = 0; i < PickUpLocations.Num(); i++)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, PickUpLocations[i], 35.f, 12, FLinearColor::Blue, 5.f, 2.f);
	}*/

	//for문 조건연산방법 (auto 까지 사용)
	for (FVector Location : PickUpLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 35.f, 12, FLinearColor::Blue, 5.f, 2.f);
	}
}

void AMainCharacter::SetMovementStatus(EMovementStatus Status)
{
    MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMainCharacter::ShiftKeyDown()
{
    bShiftKeyDown = true;
}

void AMainCharacter::ShiftKeyUp()
{
    bShiftKeyDown = false;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	UKismetSystemLibrary::DrawDebugSphere(this, GetActorLocation() + FVector(0.f, 0.f, 75.f), 35.f, 12, FLinearColor::Blue, 5.f, 2.f);
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
	
	case EStaminaStatus::ESS_Normal:
		//shiftKey Down
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
		        SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStamina;
			}
			else
			{
			    Stamina -= DeltaStamina;
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			
		}
		//shiftKeyUp
		else
		{
			if (Stamina + DeltaStamina >= MaxStamina)
			{
			    Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum:
		//shiftKey Down
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= 0.f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			
		}
		//shiftKeyUp
		else
		{
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
			    SetMovementStatus(EMovementStatus::EMS_Normal);
				Stamina += MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_Exhausted:
		//shiftKey Down
		if (bShiftKeyDown)
		{
			Stamina = 0;
		}
		//shiftKeyUp
		else
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		//shiftKey Down
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		//shiftKeyUp
		else
		{
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &AMainCharacter::StopJumping);

	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &AMainCharacter::ShiftKeyDown);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &AMainCharacter::ShiftKeyUp);

	PlayerInputComponent->BindAction(TEXT("LMB"), IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction(TEXT("LMB"), IE_Released, this, &AMainCharacter::LMBUp);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AMainCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);


}

void AMainCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.0 && (!bAttacking))
	{
	    const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.0 && (!bAttacking))
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

void AMainCharacter::DecrementHealth(float Amount)
{
	if (Health - Amount <= 0.f)
	{
	    Health -= Amount;
	    Die();
	}
	else
	{
		Health -= Amount;
	}
}

void AMainCharacter::IncrementHealth(int32 Amount)
{
	Coins += Amount;
}

void AMainCharacter::Die()
{
    
}

void AMainCharacter::LMBDown()
{
    bLMBDown = true;

	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}
	else if(EquipWeapon)
	{
	    Attack();
	}
}

void AMainCharacter::LMBUp()
{
    bLMBDown = false;
}

void AMainCharacter::SetEquipWeapon(AWeapon* WeaponToSet)
{
	if (EquipWeapon)
	{
		EquipWeapon->Destroy();
	}
    EquipWeapon = WeaponToSet; 
}

void AMainCharacter::Attack()
{

	if (!bAttacking)
	{
		bAttacking = true;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			 int32 Section = FMath::RandRange(0, 1);
			 switch (Section)
			 {
			 case 0:
				 AnimInstance->Montage_Play(CombatMontage, 2.2f);
				 AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				 UE_LOG(LogTemp, Warning, TEXT("Case 0"));
				 break;

			 case 1:
				 AnimInstance->Montage_Play(CombatMontage, 1.8f);
				 AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				 UE_LOG(LogTemp, Warning, TEXT("Case 1"));
				 break;

			 default:
				 break;
			 }
			 /*AnimInstance->Montage_Play(CombatMontage, 1.35f);
			 AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);*/
		}
	}  
	if (EquipWeapon->SwingSound)
	{
	    //UGameplayStatics::PlaySound2D(this, EquipWeapon->SwingSound);
	}
}

void AMainCharacter::AttackEnd()
{
    bAttacking = false;

	if (bLMBDown)
	{
		Attack();
	}
}

void AMainCharacter::PlaySwingSound()
{
    if(EquipWeapon->SwingSound)
	{
	     UGameplayStatics::PlaySound2D(this,EquipWeapon->SwingSound);
	}
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"



//단, 여기서 주의해야할 점이 있는데, 언리얼 열거형을 만들때 반드시 일반적인 enum이 아닌 enum class로 만들어야 한다는 점이다.
//만약 enum class로 만들지 않고 일반적인 enum으로 만들어서 UENUM() 매크로를 붙이고 컴파일을 하면 에러가 발생해서 컴파일에 실패한다.
//그리고 UENUM은 uint8만을 지원하기 때문에 이 부분도 빠뜨리지 않고 넣어주어야 한다.


UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
// UMETA = 리플렉션과 블루프린트에 쓸것

    EMS_Normal UMETA(DisplayName = "EMS_Normal"),
    EMS_Sprinting UMETA(DisplayName = "EMS_Sprinting"),
    EMS_Max UMETA(DisplayName = "EMS_DefaultMax"),

};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
    ESS_Normal UMETA(DisplayName = "ESS_Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "ESS_BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "ESS_Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ESS_ExhaustedRecovering"),

	ESS_Max UMETA(DisplayName = "ESS_DefaultMax"),

};

//Character의 rootcomp는 capsule밖에 안댐
UCLASS()
class RPG716_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	TArray<FVector> PickUpLocations;

	UFUNCTION(BlueprintCallable)
	void ShowPickUpLocation();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status;}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	//set SetMovementStatus and runnigspeed
	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintSpeed;

	bool bShiftKeyDown;

	//pressed 
	void ShiftKeyDown();
	//released 
	void ShiftKeyUp();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	////////
	// Player Stats
	////////
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//playerinput
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	
	//HUD
	void DecrementHealth(float Amount);
	void IncrementHealth(int32 Amount);
	void Die();

	//Sound
	void LMBDown();
	void LMBUp();
	bool bLMBDown;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//SetEquipWeapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquipWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;

	void SetEquipWeapon(AWeapon* WeaponToSet);
	FORCEINLINE AWeapon* GetEquipWeapon() { return EquipWeapon; }
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();
};

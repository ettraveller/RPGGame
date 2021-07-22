 // Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include <Components/SkeletalMeshComponent.h>
#include "MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include <Sound/SoundCue.h>
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


AWeapon::AWeapon()
{
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(GetRootComponent());

    bWeaponParticle = false;

    WeaponState = EWeaponState::EWS_Pickup;
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    if (WeaponState == EWeaponState::EWS_Pickup && OtherActor)
    {
        AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
        if (Main)
        {
            //Equip(Main);
            Main->SetActiveOverlappingItem(this);
        }
    }
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (OtherActor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			//Equip(Main);
			Main->SetActiveOverlappingItem(nullptr);
		}
	}
}

void AWeapon::Equip(class AMainCharacter* Char)
{
    if (Char)
    {
        //폰과 카메라와 충돌무시설정
        SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
        SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        SkeletalMesh->SetSimulatePhysics(false);

        const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
        if (RightHandSocket)
        {
            RightHandSocket->AttachActor(this, Char->GetMesh());
            bRotate = false;
            //char에 equipweapon이 null 이였는데 사용할려해서 크래쉬남
            // 대부분크래쉬는 잘못된포인터사용으로 인해남
            //Char->GetEquipWeapon()->Destroy();
            Char->SetEquipWeapon(this);
            Char->SetActiveOverlappingItem(nullptr);
        }
        if (OnEquipSound)
        {
            UGameplayStatics::PlaySound2D(this, OnEquipSound);
        }
        if (!bWeaponParticle)
        {
            IdleParticleComp->Deactivate();
        }
    }
}

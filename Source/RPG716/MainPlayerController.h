// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RPG716_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    
	//Reference to UMG asset
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;

	//위젯 생성후 변수 담을곳
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;

protected:

    virtual void BeginPlay() override;
};

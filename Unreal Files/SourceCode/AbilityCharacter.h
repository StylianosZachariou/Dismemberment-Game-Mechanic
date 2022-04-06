// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BreakableCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "AbilityCharacter.generated.h"


UCLASS()
class CMP302_API AAbilityCharacter : public ABreakableCharacter
{
	GENERATED_BODY()
		
public:
	//Constructor
	AAbilityCharacter();

	//Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget)
		UWidgetComponent* widget;

	//Collision Sphere
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
		USphereComponent* sphereReach;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

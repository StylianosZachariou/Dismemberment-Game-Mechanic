// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BreakableCharacter.h"

#include "PlainCharacter.generated.h"


UCLASS()
class CMP302_API APlainCharacter : public ABreakableCharacter
{
	GENERATED_BODY()
	
public:
	//Contructor
	APlainCharacter();

	//Customizable Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BodypartConnect)
		float moveToAbilityLocationSpeed = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BodypartConnect)
		float moveToAbilityRotationSpeed = 250;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BodypartConnect)
		float moveToBodyRotationSpeed = 250;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BodypartConnect)
		float teleportingHeight = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BodypartConnect)
		float movementSpeedX = 12000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BodypartConnect)
		float movementSpeedY = 12000;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};

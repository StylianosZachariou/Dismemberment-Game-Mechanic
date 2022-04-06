// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BodypartPawn.h"
#include "LeftArmPawn.generated.h"


UCLASS()
class CMP302_API ALeftArmPawn : public ABodypartPawn
{
	GENERATED_BODY()


public:
	//Constructor
	ALeftArmPawn();

protected:
	//Return To Body
	bool ReturnRotation(float DeltaTime, bool arrived);
	bool ReturnLocation(float DeltaTime);

	//Movement
	void MoveForward(float axis);
	void MoveRight(float axis);

	//Change Control
	void ControlHead();
	void ControlRightArm();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
};

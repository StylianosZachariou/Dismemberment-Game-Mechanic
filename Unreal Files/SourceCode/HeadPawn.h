// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BodypartPawn.h"
#include "HeadPawn.generated.h"


UCLASS()
class CMP302_API AHeadPawn : public ABodypartPawn
{
	GENERATED_BODY()

public:
	//Constructor
	AHeadPawn();


protected:
	//Return To Body
	bool ReturnRotation(float DeltaTime, bool arrived);
	bool ReturnLocation(float DeltaTime);

	//Movement
	void MoveForward(float axis);
	void MoveRight(float axis);

	//Control other body parts
	void ControlLeftArm();
	void ControlRightArm();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"

#include "BodypartPawn.generated.h"

class ABreakableCharacter;

UCLASS()
class CMP302_API ABodypartPawn : public APawn
{
	GENERATED_BODY()

public:
	//Constructor
	ABodypartPawn();

	//Camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		UCameraComponent* Camera;

	//Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		UStaticMeshComponent* Mesh;

	//Equip Socket
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Socket)
		FName EquipSocket;

	//Original Body Pointer
	UPROPERTY(VisibleAnywhere, Category = Attached)
		ABreakableCharacter* OriginalBody;

	//Customizable Variables
	float moveToAbilityLocationSpeed;
	float moveToAbilityRotationSpeed;
	float moveToBodyRotationSpeed;
	float teleportingHeight;
	float movementSpeedX;
	float movementSpeedY;

	//Control
	bool control = false;

	//Ability Body Pointer
	ABreakableCharacter* AbilityBody;

	//Connect to Ability Character
	void ConnectToAbilityCharacter();
protected:
	//The starting returning position
	FVector returnPos;

	//Camera Movement
	void Turn(float AxisValue);
	void LookUp(float AxisValue);

	//Control Change
	void ControlAbilityBody(ABreakableCharacter* body);
	void ControlBody();
	
	//Connect to character
	void ConnectToBody();
	bool ConnectLocation(float DeltaTime);
	bool ConnectRotation(float DeltaTime);

	//Connect Sequences
	bool startReturn = false;
	bool connectAbilityCharacter = false;

	//Ability Pointer
	ABreakableCharacter* LastAbility;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};


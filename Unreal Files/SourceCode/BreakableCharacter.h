// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "BodypartPawn.h"
#include "HeadPawn.h"
#include "LeftArmPawn.h"
#include "RightArmPawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "BreakableCharacter.generated.h"

UCLASS()
class CMP302_API ABreakableCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	//Constructor
	ABreakableCharacter();

	//Camera
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
		USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
		UCameraComponent* FollowCamera;

	//Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		USkeletalMeshComponent* Left_leg;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		USkeletalMeshComponent* Right_leg;

	//Body Parts
	UPROPERTY(VisibleAnywhere, Category = Attached)
		AHeadPawn* EquippedHeadPawn;

	UPROPERTY(VisibleAnywhere, Category = Attached)
		ALeftArmPawn* EquippedLeftArmPawn;

	UPROPERTY(VisibleAnywhere, Category = Attached)
		ARightArmPawn* EquippedRightArmPawn;

	UPROPERTY(VisibleAnywhere, Category = Attached)
		TSubclassOf<ABodypartPawn> AttachedPawn;

	//Customizable Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Remove)
		float removeHeadForceY = 4000;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Remove)
		float removeHeadForceZ = 2000;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Remove)
		float removeLeftArmForce = 2000;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Remove)
		float removeRightArmForce = 2000;

	//Control
	bool controlBody = true;

	//Attached variables
	bool headAttached = true;
	bool leftArmAttached = true;
	bool rightArmAttached = true;

	//Attach pawn to this
	void Attach(ABodypartPawn* Attachment, int bodypart);

	//Control Body
	void ControlBody();

protected:
	//Movement
	void moveForward(float axis);
	void moveRight(float axis);

	//Body Parts detachment
	void removeHead();
	void removeLeftArm();
	void removeRightArm();

	//Skeletal Meshes
	USkeletalMesh* bodyMesh;
	USkeletalMesh* lLegMesh;
	USkeletalMesh* rLegMesh;

	//Material
	UMaterial* material;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

};

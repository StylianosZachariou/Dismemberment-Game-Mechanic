// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableCharacter.h"

//Constructor
ABreakableCharacter::ABreakableCharacter()
{
 	// Set this character to call Tick() every frame. 
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	//Setup Camera and Spring arm
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;


	//Meshes
	Left_leg = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Left Leg Visual"));
	Left_leg->SetupAttachment(GetMesh());
	Right_leg = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Right Leg Visual"));
	Right_leg->SetupAttachment(GetMesh());
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>BodyVisualAsset(TEXT("SkeletalMesh'/Game/Mannequin/Character/Bodyparts/UE4_Mannequin_Body.UE4_Mannequin_Body'"));
	bodyMesh = BodyVisualAsset.Object;
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>LeftLegVisualAsset(TEXT("SkeletalMesh'/Game/Mannequin/Character/Bodyparts/UE4_Mannequin_LeftLeg.UE4_Mannequin_LeftLeg'"));
	lLegMesh = LeftLegVisualAsset.Object;
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>RightLegVisualAsset(TEXT("SkeletalMesh'/Game/Mannequin/Character/Bodyparts/UE4_Mannequin_RightLeg.UE4_Mannequin_RightLeg'"));
	rLegMesh = RightLegVisualAsset.Object;

	GetMesh()->SetRelativeLocation({ 0,0,-90 });
	GetMesh()->SetRelativeRotation({0,270, 0 });

	controlBody = true;
}

// Called when the game starts or when spawned
void ABreakableCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Set SKeletal Meshes
	this->GetMesh()->SetSkeletalMesh(bodyMesh);
	Left_leg->SetSkeletalMesh(lLegMesh);
	Right_leg->SetSkeletalMesh(rLegMesh);
}

// Called every frame
void ABreakableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Setup Player Input
void ABreakableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Camera Movement
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	//Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//Movement
	PlayerInputComponent->BindAxis("MoveForward", this, &ABreakableCharacter::moveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABreakableCharacter::moveRight);

	//Detachment controls
	PlayerInputComponent->BindAction("HeadOff", IE_Pressed, this, &ABreakableCharacter::removeHead);
	PlayerInputComponent->BindAction("LeftArmOff", IE_Pressed, this, &ABreakableCharacter::removeLeftArm);
	PlayerInputComponent->BindAction("RightArmOff", IE_Pressed, this, &ABreakableCharacter::removeRightArm);

}

//Move Forward
void ABreakableCharacter::moveForward(float axis)
{
	//If this character is controlled
	if (controlBody)
	{
		//Get Control Rotation
		const FRotator Rotation = Controller->GetControlRotation();

		//Get Yaw Rotation
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		//Get Direction from rotation and x-axis
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		//Add movement input towards that direction
		AddMovementInput(Direction, axis);
	}
}

//Move Right
void ABreakableCharacter::moveRight(float axis)
{
	//If this character is controlled
	if (controlBody)
	{
		//Get Control Rotation
		const FRotator Rotation = Controller->GetControlRotation();

		//Get Yaw Rotation
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		//Get Direction from rotation and y-axis
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		//Add movement input towards that direction
		AddMovementInput(Direction, axis);
	}
}

//Control this body
void ABreakableCharacter::ControlBody()
{
	//Unpause animations
	GetMesh()->bPauseAnims = false;
	Left_leg->bPauseAnims = false;
	Right_leg->bPauseAnims = false;

	//Check if head is attached before letting the body move
	if (headAttached)
	{
		controlBody = true;
	}
}

//Detach Head
void ABreakableCharacter::removeHead()
{	
	//If a head exists
	if (EquippedHeadPawn)
	{
		//If head is attaches
		if (headAttached)
		{
			//If the player is not jumping
			if (GetVelocity().Z == 0)
			{
				//Stop controlling this character
				controlBody = false;
				//Strat controlling the head pawn
				EquippedHeadPawn->control = true;

				FVector socketLoc;
				FRotator socketRot;

				//Get Sockets rotation and location
				GetMesh()->GetSocketWorldLocationAndRotation(EquippedHeadPawn->EquipSocket, socketLoc, socketRot);
				FDetachmentTransformRules rules(EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, false);
				
				//Detach Mesh
				EquippedHeadPawn->Mesh->DetachFromComponent(rules);

				//Enable physics and collisions
				EquippedHeadPawn->SetActorEnableCollision(true);
				EquippedHeadPawn->Mesh->SetSimulatePhysics(true);

				//Rotate and locate to sockets position
				EquippedHeadPawn->SetActorRotation(GetMesh()->GetComponentRotation());
				EquippedHeadPawn->SetActorLocation(socketLoc);

				//Add Force towards the direction the player is facing
				FVector imp = { 0,removeHeadForceY,removeHeadForceZ };
				FRotator rot = GetMesh()->GetComponentRotation();
				FRotationMatrix matrix = FRotationMatrix(rot);
				imp = UKismetMathLibrary::Matrix_TransformVector(matrix, imp);
				EquippedHeadPawn->Mesh->AddImpulse(imp);

				//Get Controller
				APlayerController* controller = GetWorld()->GetFirstPlayerController();
				//Unpossess this character and possess head pawn
				controller->UnPossess();
				controller->Possess(EquippedHeadPawn);

				//Re start animation sequence and pause
				GetMesh()->InitAnim(true);
				GetMesh()->bPauseAnims = true;
				Left_leg->InitAnim(true);
				Left_leg->bPauseAnims = true;
				Right_leg->InitAnim(true);
				Right_leg->bPauseAnims = true;

				//Head is no longer attached
				headAttached = false;
			}
		}
		else //If head is not attached
		{
			//Get Controller
			APlayerController* controller = GetWorld()->GetFirstPlayerController();
			//Unpossess this character and possess the head
			controller->UnPossess();
			controller->Possess(EquippedHeadPawn);

			//Switch controls
			EquippedHeadPawn->control = true;
			controlBody = false;

			//Restart animation and pause
			GetMesh()->InitAnim(true);
			GetMesh()->bPauseAnims = true;
			Left_leg->InitAnim(true);
			Left_leg->bPauseAnims = true;
			Right_leg->InitAnim(true);
			Right_leg->bPauseAnims = true;
		}
	}
}

//Detach Left Arm
void ABreakableCharacter::removeLeftArm()
{
	//If left arm exists
	if (EquippedLeftArmPawn)
	{
		//If left arm is attached
		if (leftArmAttached)
		{
			//If the character is not jumping
			if (GetVelocity().Z == 0)
			{
				//Stop controlling this character
				controlBody = false;
				//Start controlling pawn
				EquippedLeftArmPawn->control = true;

				FVector socketLoc;
				FRotator socketRot;

				//Get sockets rotation and location
				GetMesh()->GetSocketWorldLocationAndRotation(EquippedLeftArmPawn->EquipSocket, socketLoc, socketRot);
				FDetachmentTransformRules rules(EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, false);

				//Mesh is detached
				EquippedLeftArmPawn->Mesh->DetachFromComponent(rules);

				//Enable collisions and physics
				EquippedLeftArmPawn->SetActorEnableCollision(true);
				EquippedLeftArmPawn->Mesh->SetSimulatePhysics(true);

				//Rotate and locate to socket location
				EquippedLeftArmPawn->SetActorRotation(GetMesh()->GetComponentRotation());
				EquippedLeftArmPawn->SetActorLocation(socketLoc);

				//Rotate and add impulse towards the direction the character is facing
				FVector imp = { removeLeftArmForce,0,0 };
				FRotator rot = GetMesh()->GetComponentRotation();
				FRotationMatrix matrix = FRotationMatrix(rot);
				imp = UKismetMathLibrary::Matrix_TransformVector(matrix, imp);
				EquippedLeftArmPawn->Mesh->AddImpulse(imp);

				//Get Controller
				APlayerController* controller = GetWorld()->GetFirstPlayerController();
				//Unpossess this character and possess left arm pawn
				controller->UnPossess();
				controller->Possess(EquippedLeftArmPawn);

				//Restart animation and pause
				GetMesh()->InitAnim(true);
				GetMesh()->bPauseAnims = true;
				Left_leg->InitAnim(true);
				Left_leg->bPauseAnims = true;
				Right_leg->InitAnim(true);
				Right_leg->bPauseAnims = true;
				leftArmAttached = false;
			}
		}
		else // If Left armm pawn is not attached
		{
			//Get Controller
			APlayerController* controller = GetWorld()->GetFirstPlayerController();
			//Unpossess this character and possess left arm pawn
			controller->UnPossess();
			controller->Possess(EquippedLeftArmPawn);

			//Switch controlls
			EquippedLeftArmPawn->control = true;
			controlBody = false;

			//Restart and pause animations
			GetMesh()->InitAnim(true);
			GetMesh()->bPauseAnims = true;
			Left_leg->InitAnim(true);
			Left_leg->bPauseAnims = true;
			Right_leg->InitAnim(true);
			Right_leg->bPauseAnims = true;
		}
	}
}

//Detach Right Arm
void ABreakableCharacter::removeRightArm()
{
	//If the right arm pawn exists
	if (EquippedRightArmPawn)
	{
		//If the right arm pawn is attached
		if (rightArmAttached)
		{
			//If the character is onot jumping
			if (GetVelocity().Z == 0)
			{
				//Stop controlling this character
				controlBody = false;
				//Start controlling the right arm pawn
				EquippedRightArmPawn->control = true;

				FVector socketLoc;
				FRotator socketRot;

				//Get socket rotation and location
				GetMesh()->GetSocketWorldLocationAndRotation(EquippedRightArmPawn->EquipSocket, socketLoc, socketRot);
				FDetachmentTransformRules rules(EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, false);

				//Detach mesh
				EquippedRightArmPawn->Mesh->DetachFromComponent(rules);

				//Enable collisions and phyisics
				EquippedRightArmPawn->SetActorEnableCollision(true);
				EquippedRightArmPawn->Mesh->SetSimulatePhysics(true);

				//Rotate and locate to sockets location
				EquippedRightArmPawn->SetActorRotation(GetMesh()->GetComponentRotation());
				EquippedRightArmPawn->SetActorLocation(socketLoc);

				//Rotate impulse and apply it so that the impulse faces the character's forward direction
				FVector imp = { removeRightArmForce,0,0 };
				FRotator rot = GetMesh()->GetComponentRotation();
				FRotationMatrix matrix = FRotationMatrix(rot);
				imp = UKismetMathLibrary::Matrix_TransformVector(matrix, imp);
				EquippedRightArmPawn->Mesh->AddImpulse(imp);

				//Get Controller
				APlayerController* controller = GetWorld()->GetFirstPlayerController();
				//Unpossess this character and possess right arm pawn
				controller->UnPossess();
				controller->Possess(EquippedRightArmPawn);

				//Restart and pause animations
				GetMesh()->InitAnim(true);
				GetMesh()->bPauseAnims = true;
				Left_leg->InitAnim(true);
				Left_leg->bPauseAnims = true;
				Right_leg->InitAnim(true);
				Right_leg->bPauseAnims = true;
				rightArmAttached = false;
			}
		}
		else
		{
			//Get Controller
			APlayerController* controller = GetWorld()->GetFirstPlayerController();
			//Unpossess this character and possess the right arm pawn
			controller->UnPossess();
			controller->Possess(EquippedRightArmPawn);

			//Switch controlls
			EquippedRightArmPawn->control = true;
			controlBody = false;

			//Restart and pause animations
			GetMesh()->InitAnim(true);
			GetMesh()->bPauseAnims = true;
			Left_leg->InitAnim(true);
			Left_leg->bPauseAnims = true;
			Right_leg->InitAnim(true);
			Right_leg->bPauseAnims = true;
		}
	}
}

//Attach body part to this body
void ABreakableCharacter::Attach(ABodypartPawn* newAttachment,int bodypart)
{
	//Attachment Rules
	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true);

	
	if (bodypart == 1) //If Body part is head
	{
		//Attach body part to component
		EquippedHeadPawn = (AHeadPawn*)newAttachment;
		EquippedHeadPawn->AttachToComponent(GetMesh(), rules, EquippedHeadPawn->EquipSocket);
		headAttached = true;

		//Control body
		ControlBody();
	}
	else if(bodypart==2) //If body part is left arm
	{
		//Attach body part to component
		EquippedLeftArmPawn = (ALeftArmPawn*)newAttachment;
		EquippedLeftArmPawn->AttachToComponent(GetMesh(), rules, EquippedLeftArmPawn->EquipSocket);
		leftArmAttached = true;
	}
	else if(bodypart==3) //If body part is right arm
	{
		//Attach body part to component
		EquippedRightArmPawn = (ARightArmPawn*)newAttachment;
		EquippedRightArmPawn->AttachToComponent(GetMesh(), rules, EquippedRightArmPawn->EquipSocket);
		rightArmAttached = true;
	}
		
}
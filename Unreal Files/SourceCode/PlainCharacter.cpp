// Fill out your copyright notice in the Description page of Project Settings.


#include "PlainCharacter.h"

//Constructor
APlainCharacter::APlainCharacter()
{
	//Material
	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/Mannequin/Character/Materials/M_Male_Body.M_Male_Body'"));
	material = Material.Object;
}

// Called when the game starts or when spawned
void APlainCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Set SKletal Meshes
	this->GetMesh()->SetSkeletalMesh(bodyMesh);
	Left_leg->SetSkeletalMesh(lLegMesh);
	Right_leg->SetSkeletalMesh(rLegMesh);

	//Spawn and Attach Head
	FActorSpawnParameters spawnParams;
	FTransform transform;
	transform.SetLocation({ -300,-350,500 });
	FRotator rot = { 0,0,0 };
	transform.SetRotation(rot.Quaternion());
	AttachedPawn = AHeadPawn::StaticClass();
	EquippedHeadPawn = GetWorld()->SpawnActor<AHeadPawn>(AttachedPawn, transform.GetLocation(), transform.GetRotation().Rotator(), spawnParams);
	EquippedHeadPawn->SetActorEnableCollision(false);
	EquippedHeadPawn->DisableComponentsSimulatePhysics();
	EquippedHeadPawn->moveToAbilityLocationSpeed = moveToAbilityLocationSpeed;
	EquippedHeadPawn->moveToAbilityRotationSpeed = moveToAbilityRotationSpeed;
	EquippedHeadPawn->moveToBodyRotationSpeed = moveToBodyRotationSpeed;
	EquippedHeadPawn->teleportingHeight = teleportingHeight;
	EquippedHeadPawn->movementSpeedX = movementSpeedX;
	EquippedHeadPawn->movementSpeedY = movementSpeedY;

	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true);
	EquippedHeadPawn->AttachToComponent(GetMesh(), rules, EquippedHeadPawn->EquipSocket);
	EquippedHeadPawn->OriginalBody = this;


	//Spawn and Attach Left Arm	
	AttachedPawn = ALeftArmPawn::StaticClass();
	EquippedLeftArmPawn = GetWorld()->SpawnActor<ALeftArmPawn>(AttachedPawn, transform.GetLocation(), transform.GetRotation().Rotator(), spawnParams);
	EquippedLeftArmPawn->SetActorEnableCollision(false);
	EquippedLeftArmPawn->DisableComponentsSimulatePhysics();
	EquippedLeftArmPawn->AttachToComponent(GetMesh(), rules, EquippedLeftArmPawn->EquipSocket);
	EquippedLeftArmPawn->OriginalBody = this;

	EquippedLeftArmPawn->moveToAbilityLocationSpeed = moveToAbilityLocationSpeed;
	EquippedLeftArmPawn->moveToAbilityRotationSpeed = moveToAbilityRotationSpeed;
	EquippedLeftArmPawn->moveToBodyRotationSpeed = moveToBodyRotationSpeed;
	EquippedLeftArmPawn->teleportingHeight = teleportingHeight;
	EquippedLeftArmPawn->movementSpeedX = movementSpeedX;
	EquippedLeftArmPawn->movementSpeedY = movementSpeedY;

	//Spawn and Attach Right Arm
	AttachedPawn = ARightArmPawn::StaticClass();
	EquippedRightArmPawn = GetWorld()->SpawnActor<ARightArmPawn>(AttachedPawn, transform.GetLocation(), transform.GetRotation().Rotator(), spawnParams);
	EquippedRightArmPawn->SetActorEnableCollision(false);
	EquippedRightArmPawn->DisableComponentsSimulatePhysics();
	EquippedRightArmPawn->AttachToComponent(GetMesh(), rules, EquippedRightArmPawn->EquipSocket);
	EquippedRightArmPawn->OriginalBody = this;

	EquippedRightArmPawn->moveToAbilityLocationSpeed = moveToAbilityLocationSpeed;
	EquippedRightArmPawn->moveToAbilityRotationSpeed = moveToAbilityRotationSpeed;
	EquippedRightArmPawn->moveToBodyRotationSpeed = moveToBodyRotationSpeed;
	EquippedRightArmPawn->teleportingHeight = teleportingHeight;
	EquippedRightArmPawn->movementSpeedX = movementSpeedX;
	EquippedRightArmPawn->movementSpeedY = movementSpeedY;

	//SetMaterial
	UMaterialInstanceDynamic* matInstance;
	matInstance = UMaterialInstanceDynamic::Create(material, GetMesh());
	Left_leg->SetMaterial(0, matInstance);
	Right_leg->SetMaterial(0, matInstance);
	GetMesh()->SetMaterial(0, matInstance);
	GetMesh()->SetMaterial(1, matInstance);
	EquippedHeadPawn->Mesh->SetMaterial(0, matInstance);
	EquippedLeftArmPawn->Mesh->SetMaterial(0, matInstance);
	EquippedRightArmPawn->Mesh->SetMaterial(0, matInstance);
}



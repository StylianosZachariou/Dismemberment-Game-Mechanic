// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpingCharacter.h"

//Constructor
AJumpingCharacter::AJumpingCharacter()
{
	//Set a very high jump height
	GetCharacterMovement()->JumpZVelocity = 2000;

	//Setup material
	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/StarterContent/Materials/M_Metal_Gold.M_Metal_Gold'"));
	material = Material.Object;
}

// Called when the game starts or when spawned
void AJumpingCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Set skeletal meshes
	this->GetMesh()->SetSkeletalMesh(bodyMesh);
	Left_leg->SetSkeletalMesh(lLegMesh);
	Right_leg->SetSkeletalMesh(rLegMesh);


	//SetMaterial
	UMaterialInstanceDynamic* matInstance;
	matInstance = UMaterialInstanceDynamic::Create(material, GetMesh());
	Left_leg->SetMaterial(0, matInstance);
	Right_leg->SetMaterial(0, matInstance);
	GetMesh()->SetMaterial(0, matInstance);
	GetMesh()->SetMaterial(1, matInstance);

	//Set overlap events of sphere
	sphereReach->OnComponentBeginOverlap.AddDynamic(this, &AJumpingCharacter::OnOverlapBegin);
	sphereReach->OnComponentEndOverlap.AddDynamic(this, &AJumpingCharacter::OnOverlapEnd);

}

//When Overlap Begins
void AJumpingCharacter::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//If overlapped actor is a head pawn
	if (OtherActor->GetClass() == AHeadPawn::StaticClass())
	{
		//Set Widget to visible
		widget->SetVisibility(true);

		//Make the head ability body pointer to point to this character
		AHeadPawn* headPawn = (AHeadPawn*)OtherActor;
		headPawn->AbilityBody = this;
	}

}

//When Overlap Ends
void AJumpingCharacter::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//If overlapped actor is a head pawn
	if (OtherActor->GetClass() == AHeadPawn::StaticClass())
	{
		//Set Widget to invisible
		widget->SetVisibility(false);

		//Make the head ability body pointer to point to Null
		AHeadPawn* headPawn = (AHeadPawn*)OtherActor;
		headPawn->AbilityBody = NULL;
	}
}
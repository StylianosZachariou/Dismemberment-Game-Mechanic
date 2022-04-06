// Fill out your copyright notice in the Description page of Project Settings.


#include "StrongCharacter.h"

//Constructor
AStrongCharacter::AStrongCharacter()
{
	//Set Jump Hight to very low
	GetCharacterMovement()->JumpZVelocity = 100;

	//Get Material
	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/StarterContent/Materials/M_Metal_Burnished_Steel.M_Metal_Burnished_Steel'"));
	material = Material.Object;

	//Widget Setup
	lawidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Left Arm"));
	lawidget->SetupAttachment(GetMesh());

	rawidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Right Arm"));
	rawidget->SetupAttachment(GetMesh());
	
}

//When Overlap Begins
void AStrongCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Check what class is the overlapped actor
	if (OtherActor->GetClass() == AHeadPawn::StaticClass()) //Head
	{
		//Set Widget Visible
		widget->SetVisibility(true);
		
		//Set Pointer to body part
		AHeadPawn* headPawn = (AHeadPawn*)OtherActor;
		headPawn->AbilityBody = this;
	}
	else if (OtherActor->GetClass() == ALeftArmPawn::StaticClass())//Left Arm
	{
		//Set Widget Visible
		lawidget->SetVisibility(true);

		//Set pointer to body part 
		ALeftArmPawn* leftPawn = (ALeftArmPawn*)OtherActor;
		leftPawn->AbilityBody = this;
	}
	else if (OtherActor->GetClass() == ARightArmPawn::StaticClass())// Right Arm
	{
		//Set WIdget Visible
		rawidget->SetVisibility(true);

		//Set pointer to body part
		ARightArmPawn* rightPawn = (ARightArmPawn*)OtherActor;
		rightPawn->AbilityBody = this;
	}

}

//When Overlap Ends
void AStrongCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//Check what class is the overlapped actor
	if (OtherActor->GetClass() == AHeadPawn::StaticClass())
	{
		//Set Widget invisible
		widget->SetVisibility(false);

		//Set body  part pointer to null
		AHeadPawn* headPawn = (AHeadPawn*)OtherActor;
		headPawn->AbilityBody = NULL;
	}
	else if (OtherActor->GetClass() == ALeftArmPawn::StaticClass())
	{
		//Set Widget invisible
		lawidget->SetVisibility(false);

		//Set body  part pointer to null
		ALeftArmPawn* leftPawn = (ALeftArmPawn*)OtherActor;
		leftPawn->AbilityBody = NULL;
	}
	else if (OtherActor->GetClass() == ARightArmPawn::StaticClass())
	{
		//Set Widget invisible
		rawidget->SetVisibility(false);		
		
		//Set body  part pointer to null
		ARightArmPawn* rightPawn = (ARightArmPawn*)OtherActor;
		rightPawn->AbilityBody = NULL;
	}
}

//Remove All Body Parts
void AStrongCharacter::removeAll()
{
	//Detach left arm
	if (leftArmAttached)
	{
		removeLeftArm();
	}

	//Detach right arm
	if (rightArmAttached)
	{
		removeRightArm();
	}
	
	//Detach head
	if (headAttached)
	{
		removeHead();
	}
}

//Called when begin play
void AStrongCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Set Skeletal Meshes
	this->GetMesh()->SetSkeletalMesh(bodyMesh);
	Left_leg->SetSkeletalMesh(lLegMesh);
	Right_leg->SetSkeletalMesh(rLegMesh);

	//Set the widget visibility to invisible
	lawidget->SetVisibility(false);
	rawidget->SetVisibility(false);

	//SetMaterial
	UMaterialInstanceDynamic* matInstance;
	matInstance = UMaterialInstanceDynamic::Create(material, GetMesh());
	Left_leg->SetMaterial(0, matInstance);
	Right_leg->SetMaterial(0, matInstance);
	GetMesh()->SetMaterial(0, matInstance);
	GetMesh()->SetMaterial(1, matInstance);

	//Set Overlap events of sphere
	sphereReach->OnComponentBeginOverlap.AddDynamic(this, &AStrongCharacter::OnOverlapBegin);
	sphereReach->OnComponentEndOverlap.AddDynamic(this, &AStrongCharacter::OnOverlapEnd);
}

//For every tick
void AStrongCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//If Both arms are attached
	if (leftArmAttached && rightArmAttached)
	{
		//Character can push
		canPush = true;
	}
	else
	{
		//Can't push
		canPush = false;
	}

	//If the body is controlled
	if (controlBody)
	{
		//Turn off collision sphere
		sphereReach->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		//Turn on collision sphere
		sphereReach->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	}

	//If player can push
	if (canPush)
	{
		//If is Pushing
		if (isPushing)
		{
			//Movement speed is lowered
			GetCharacterMovement()->MaxWalkSpeed = 200;
		}
		else
		{
			//Movement speed is increased
			GetCharacterMovement()->MaxWalkSpeed = 600;
		}
	}
}

//Set Up User Input
void AStrongCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//When one body part is removed, all are removed
	PlayerInputComponent->BindAction("HeadOff", IE_Pressed, this, &AStrongCharacter::removeAll);
	PlayerInputComponent->BindAction("LeftArmOff", IE_Pressed, this, &AStrongCharacter::removeAll);
	PlayerInputComponent->BindAction("RightArmOff", IE_Pressed, this, &AStrongCharacter::removeAll);
}

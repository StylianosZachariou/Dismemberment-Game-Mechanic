// Fill out your copyright notice in the Description page of Project Settings.


#include "MovableObject.h"

//Constructor
AMovableObject::AMovableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Mesh Setup
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshVisualAsset(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube.1M_Cube'"));
	Mesh->SetStaticMesh(MeshVisualAsset.Object);
	Mesh->SetMassOverrideInKg(NAME_None, 10000);
	Mesh->SetLinearDamping(10);

	//Box Collision Set Up
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);
	Box->SetRelativeScale3D({ 1.815,1.815 ,1.815 });
	Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Box->SetGenerateOverlapEvents(true);
	Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	//Physics Constraint Setup
	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Physics Constraint"));
	PhysicsConstraint->SetupAttachment(RootComponent);
	PhysicsConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Locked,1);
	PhysicsConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Free, 1);
	PhysicsConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Free, 1);
	PhysicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 1);	
	PhysicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 1);
	PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 1);
	PhysicsConstraint->SetConstrainedComponents(Mesh, FName("Mesh"),NULL,NAME_None);
	
	//Material Set up
	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/StarterContent/Materials/M_Brick_Cut_Stone.M_Brick_Cut_Stone'"));
	material = Material.Object;

}


//When Overlap Begins
void AMovableObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Check if overlapped character is Strong Character
	AStrongCharacter* strongCharacter = Cast<AStrongCharacter>(OtherActor);
	if (strongCharacter)
	{
		//Character pointer points to overlapped actor
		character = strongCharacter;
	}
}

//When Overlap Ends
void AMovableObject::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//If actor that stopped overlap is the character
	if (OtherActor == character)
	{
		//Then stops pushing
		character->isPushing = false;
		//Character pointer points to NULL
		character = NULL;
	}
}

// Called when the game starts or when spawned
void AMovableObject::BeginPlay()
{
	Super::BeginPlay();
	//Simulate Physics
	Mesh->SetSimulatePhysics(true);
	
	//Material
	UMaterialInstanceDynamic* matInstance;
	matInstance = UMaterialInstanceDynamic::Create(material, Mesh);
	Mesh->SetMaterial(0, matInstance);

	//Set Overlap Functions
	Box->OnComponentBeginOverlap.AddDynamic(this, &AMovableObject::OnOverlapBegin);
	Box->OnComponentEndOverlap.AddDynamic(this, &AMovableObject::OnOverlapEnd);
}

// Called every frame
void AMovableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//If a character is in range
	if (character)
	{
		//If the character can push
		if (character->canPush)
		{
			//If the character is moving
			if (character->GetVelocity().X !=0 || character->GetVelocity().Y !=0)
			{
				//Character is pushing and Object mass is decreased
				character->isPushing = true;
				Mesh->SetMassOverrideInKg(NAME_None, 300);
			}
			else
			{
				//Character stops pushing and Object mass is increased
				character->isPushing = false;
				Mesh->SetMassOverrideInKg(NAME_None, 10000);
			}
		}		
	}
	else
	{
		//Object mass is increased
		Mesh->SetMassOverrideInKg(NAME_None, 10000);
	}
}


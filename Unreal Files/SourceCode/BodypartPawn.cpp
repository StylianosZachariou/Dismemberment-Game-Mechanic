// Fill out your copyright notice in the Description page of Project Settings.


#include "BodypartPawn.h"
#include "BreakableCharacter.h"

//Constructor
ABodypartPawn::ABodypartPawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    //Mesh Set Up
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
    RootComponent = Mesh;
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);

    //Spring Arm Setup
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritRoll = false;
    SpringArm->bInheritYaw = false;
    SpringArm->TargetArmLength = 300.0f;
    SpringArm->SetRelativeRotation(FRotator(-30.0f,90.f, 00.f));
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->bDoCollisionTest = false;

    //Camera Set-up
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ABodypartPawn::BeginPlay()
{
    Super::BeginPlay();
    //Simulate Physics
    Mesh->SetSimulatePhysics(true);
}

// Called every frame
void ABodypartPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

//Connect This to its original Body
void ABodypartPawn::ConnectToBody()
{
    //Start return sequence
    startReturn = true;

    //Stop simulating Physics or Collisions
    Mesh->SetSimulatePhysics(false);
    SetActorEnableCollision(false);

    returnPos = GetActorLocation();

    //Stop Controlling this body part
    control = false;
}

//Set Up Player Input
void ABodypartPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    //Camera Movement
    PlayerInputComponent->BindAxis("Turn", this, &ABodypartPawn::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &ABodypartPawn::LookUp);

    //Control Body
    PlayerInputComponent->BindAction("ControlBody",IE_Pressed, this, &ABodypartPawn::ControlBody);
}

//Turn Camera
void ABodypartPawn::Turn(float AxisValue)
{
    //If this body part is controlled
    if (control)
    {
        //Add Yawn input to the controller
        AddControllerYawInput(AxisValue);
        //Get the controller rotation
        FRotator Rotation = Controller->GetControlRotation();
        //Set the spring arm's rotation Yaw, to the controllers rotation
        SpringArm->SetRelativeRotation({ SpringArm->GetRelativeRotation().Pitch, Rotation.Yaw+ 90, 0 });
    }
}

//LookUp Camera
void ABodypartPawn::LookUp(float AxisValue)
{
    //If this body part is controlled
    if (control)
    {
        //Add Pitch input to the controller
        AddControllerPitchInput(AxisValue);
        //Get the controller rotation
        FRotator Rotation = Controller->GetControlRotation();
        //Set the spring arm's rotation Pitch, to the controllers rotation
        SpringArm->SetRelativeRotation({ Rotation.Pitch - 20.0f, SpringArm->GetRelativeRotation().Yaw , 0 });
    }
}

//Control the given ability body
void ABodypartPawn::ControlAbilityBody(ABreakableCharacter* body)
{
    //Get Controller
    APlayerController* controller = GetWorld()->GetFirstPlayerController();

    //Unpossess this character and possess the ability body.
    controller->UnPossess();
    controller->Possess(body);

    //Control that body
    body->ControlBody();

    //Stop Controlling this body part
    control = false;

}

//Control original Body
void ABodypartPawn::ControlBody()
{
    //Get controller
    APlayerController* controller = GetWorld()->GetFirstPlayerController();
    
    //Unpossess this pawn and possess the original body character
    controller->UnPossess();
    controller->Possess(OriginalBody);

    //Control that character
    OriginalBody->ControlBody();

    //Stop controlling this character
    control = false;
}

//Connect to an ability character location
bool ABodypartPawn::ConnectLocation(float DeltaTime)
{
    //Get the sockets location
    FVector bodyLocation = LastAbility->GetMesh()->GetSocketLocation(EquipSocket);
    //Calculate movement speed
    float moveAmount = DeltaTime * moveToAbilityLocationSpeed;

    //Move towards the body's x-coordinate
    if (GetActorLocation().X < bodyLocation.X - moveAmount)
    {
        SetActorLocation({ GetActorLocation().X + moveAmount, GetActorLocation().Y,GetActorLocation().Z });
    }
    else if (GetActorLocation().X > bodyLocation.X + moveAmount)
    {
        SetActorLocation({ GetActorLocation().X - moveAmount,GetActorLocation().Y,GetActorLocation().Z });
    }
    else
    {
        SetActorLocation({ bodyLocation.X,GetActorLocation().Y,GetActorLocation().Z });
    }

    //Move towards the body's y-coordinate
    if (GetActorLocation().Y < bodyLocation.Y - moveAmount )
    {
        SetActorLocation({ GetActorLocation().X , GetActorLocation().Y + moveAmount,GetActorLocation().Z });
    }
    else if (GetActorLocation().Y > bodyLocation.Y + moveAmount )
    {
        SetActorLocation({ GetActorLocation().X ,GetActorLocation().Y - moveAmount,GetActorLocation().Z });
    }
    else
    {
        SetActorLocation({ GetActorLocation().X, bodyLocation.Y,GetActorLocation().Z });
    }

    //Move towards the body's z-coordinate
    if (GetActorLocation().Z < bodyLocation.Z - moveAmount )
    {
        SetActorLocation({ GetActorLocation().X , GetActorLocation().Y,GetActorLocation().Z + moveAmount });
    }
    else if (GetActorLocation().Z > bodyLocation.Z + moveAmount )
    {
        SetActorLocation({ GetActorLocation().X ,GetActorLocation().Y,GetActorLocation().Z - moveAmount });
    }
    else
    {
        SetActorLocation({ GetActorLocation().X,GetActorLocation().Y,bodyLocation.Z });
    }

    //If the body part's location matches the body's location
    if (bodyLocation == GetActorLocation())
    {
        return true;
    }
    return false;
}

//Connect to an ability character rotation
bool ABodypartPawn::ConnectRotation(float DeltaTime)
{
    //Get Sockets rotation
    FRotator bodyRotation = LastAbility->GetMesh()->GetSocketRotation(EquipSocket);
    //Calculate rotation amount
    float rotateAmount = DeltaTime * moveToAbilityRotationSpeed;

    //Turn towards body's pitch
    if (GetActorRotation().Pitch < bodyRotation.Pitch - rotateAmount)
    {
        SetActorRotation({ GetActorRotation().Pitch + rotateAmount,GetActorRotation().Yaw,GetActorRotation().Roll });
    }
    else if (GetActorRotation().Pitch > bodyRotation.Pitch + rotateAmount)
    {
        SetActorRotation({ GetActorRotation().Pitch - rotateAmount,GetActorRotation().Yaw,GetActorRotation().Roll });
    }
    else
    {
        SetActorRotation({ bodyRotation.Pitch,GetActorRotation().Yaw,GetActorRotation().Roll });
    }

    //Turn towards body's yaw
    if (GetActorRotation().Yaw < bodyRotation.Yaw - rotateAmount)
    {
        SetActorRotation({ GetActorRotation().Pitch ,GetActorRotation().Yaw + rotateAmount,GetActorRotation().Roll });
    }
    else if (GetActorRotation().Yaw > bodyRotation.Yaw + rotateAmount)
    {
        SetActorRotation({ GetActorRotation().Pitch ,GetActorRotation().Yaw - rotateAmount,GetActorRotation().Roll });
    }
    else
    {
        SetActorRotation({ GetActorRotation().Pitch,bodyRotation.Yaw,GetActorRotation().Roll });
    }

    //Turn towards body's rool
    if (GetActorRotation().Roll < bodyRotation.Roll - rotateAmount)
    {
        SetActorRotation({ GetActorRotation().Pitch ,GetActorRotation().Yaw ,GetActorRotation().Roll + rotateAmount });
    }
    else if (GetActorRotation().Roll > bodyRotation.Roll + rotateAmount)
    {
        SetActorRotation({ GetActorRotation().Pitch ,GetActorRotation().Yaw ,GetActorRotation().Roll - rotateAmount });
    }
    else
    {
        SetActorRotation({ GetActorRotation().Pitch,GetActorRotation().Yaw,bodyRotation.Roll });
    }

    //If body part's rotation is the same as the body's rotation
    if (bodyRotation.Equals(GetActorRotation(), 1))
    {
        return true;
    }
    return false;
}

//Connect to ability character
void ABodypartPawn::ConnectToAbilityCharacter()
{
    //If there is an ability Body in range
    if (AbilityBody)
    {
        //Start connect to ability character sequence
        connectAbilityCharacter = true;

        //Stop simulating physics and collisions
        Mesh->SetSimulatePhysics(false);
        SetActorEnableCollision(false);

        returnPos = GetActorLocation();
        //Save Last ability
        LastAbility = AbilityBody;

        //Stp Controlling this body part
        control = false;
    }
}
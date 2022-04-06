// Fill out your copyright notice in the Description page of Project Settings.


#include "RightArmPawn.h"
#include "BreakableCharacter.h"

//Constructor
ARightArmPawn::ARightArmPawn()
{
    //Set Right Arm Mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh>RightArmVisual(TEXT("StaticMesh'/Game/Mannequin/Meshes/RightArm_Mesh.RightArm_Mesh'"));
    Mesh->SetStaticMesh(RightArmVisual.Object);
    Mesh->SetRelativeScale3D({ 0.01,0.01,0.01 });

    //Equip Socket
    EquipSocket = FName("rightArmSocket");

    //Damping
    Mesh->SetLinearDamping(1);

}

//Return to Original Body's Rotaiton
bool ARightArmPawn::ReturnRotation(float DeltaTime, bool arrived)
{
    //Get Sockets rotation
    FRotator bodyRotation = OriginalBody->GetMesh()->GetSocketRotation(EquipSocket);
    //Calculate rotate amount
    float rotateAmount = DeltaTime * moveToBodyRotationSpeed;

    //If location is arrived
    if (arrived)
    {
        //Turn to Correct Pitch
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

        //Turn to correct Yaw
        if (GetActorRotation().Yaw < bodyRotation.Yaw - rotateAmount)
        {
            SetActorRotation({ GetActorRotation().Pitch ,GetActorRotation().Yaw + rotateAmount,GetActorRotation().Roll });
        }
        else if (GetActorRotation().Yaw > bodyRotation.Yaw + rotateAmount)
        {
            SetActorRotation({ GetActorRotation().Pitch ,GetActorRotation().Yaw + rotateAmount,GetActorRotation().Roll });
        }
        else
        {
            SetActorRotation({ GetActorRotation().Pitch,bodyRotation.Yaw,GetActorRotation().Roll });
        }

        //Turn to Connect Roll
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
    }
    else
    {
        //Continuesly Rotate aroun Y-axis
        SetActorRotation({ GetActorRotation().Pitch ,GetActorRotation().Yaw + rotateAmount ,GetActorRotation().Roll });
    }

    //If Rotation is the same as the original bodies
    if (bodyRotation.Equals(GetActorRotation(), 1))
    {
        return true;
    }
    return false;
}

//Return to Original Body's Location
bool ARightArmPawn::ReturnLocation(float DeltaTime)
{
    //Get Socket's Location
    FVector bodyLocation = OriginalBody->GetMesh()->GetSocketLocation(EquipSocket);

    float highestPoint = 0;
    float movementSpeed = 0;

    //Get Highest point and movement speed
    if (bodyLocation.Z > returnPos.Z)
    {
        highestPoint = bodyLocation.Z + teleportingHeight;
        movementSpeed = (highestPoint - returnPos.Z);
    }
    else
    {
        highestPoint = returnPos.Z + teleportingHeight;
        movementSpeed = (highestPoint - bodyLocation.Z);
    }

    //If it did not teleport
    if (!(GetActorLocation().X == bodyLocation.X) || !(GetActorLocation().Y == bodyLocation.Y))
    {
        //If location is less than highest point
        if (GetActorLocation().Z < highestPoint)
        {
            //Continue moving up
            SetActorLocation({ GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + movementSpeed * DeltaTime });
        }
        else
        {
            //Teleport
            SetActorLocation({ bodyLocation.X, bodyLocation.Y,  GetActorLocation().Z + movementSpeed * DeltaTime });
        }
    }
    else
    {
        //If position is less that bodys position
        if (GetActorLocation().Z <= bodyLocation.Z)
        {
            //Set to Body's position
            SetActorLocation(bodyLocation);
        }
        else
        {
            //Move towards the negatice z-axis
            SetActorLocation({ bodyLocation.X, bodyLocation.Y, GetActorLocation().Z - movementSpeed * DeltaTime });
        }
    }

    //If body part location is the same as the body
    if (bodyLocation == GetActorLocation())
    {
        return true;
    }

    return false;
}

//Move Forward
void ARightArmPawn::MoveForward(float AxisValue)
{
    //If this body part is controleld
    if (control)
    {
        // Get Controller Rotation
        const FRotator Rotation = Controller->GetControlRotation();

        //GET Yaw rotation
        const FRotator YawRotation(0, Rotation.Yaw + 90, 0);

        //Get Direction using rotation and the x-axis
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        //Add force towards that direction
        Mesh->AddForce({ (Direction.X) * AxisValue * movementSpeedX ,(Direction.Y) * AxisValue * movementSpeedY ,0 });
    }
}

//Move Right
void ARightArmPawn::MoveRight(float AxisValue)
{
    //If this body part is controlled
    if (control)
    {
        //Get Controller Rotation
        const FRotator Rotation = Controller->GetControlRotation();
        //Get Yaw Rotation
        const FRotator YawRotation(0, Rotation.Yaw + 90, 0);

        //Get Direction Using Rotation and y-axis
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        //Add Force towards that direction
        Mesh->AddForce({ (Direction.X) * AxisValue * movementSpeedX  ,(Direction.Y) * AxisValue * movementSpeedY ,0 });
    }
}

//Control Left Arm
void ARightArmPawn::ControlLeftArm()
{
    //If the body part is not returning to its original body
    if (!startReturn)
    {
        //If the left arm exists but is not attached
        if (OriginalBody->EquippedLeftArmPawn && !OriginalBody->leftArmAttached)
        {
            //Stop controlling this body part
            control = false;
            //Get Controller
            APlayerController* controller = GetWorld()->GetFirstPlayerController();
            //Unposses this pawn and posses the left arm pawn
            controller->UnPossess();
            controller->Possess(OriginalBody->EquippedLeftArmPawn);
            //Turn the control of the left arm on
            OriginalBody->EquippedLeftArmPawn->control = true;
        }
    }
}

//Control RightArm
void ARightArmPawn::ControlHead()
{
    //If the body part is not returning to its original body
    if (!startReturn)
    {
        //If the head exists but is not attached
        if (OriginalBody->EquippedHeadPawn && !OriginalBody->headAttached)
        {
            //Turn control off for this pawn
            control = false;
            //Get Player Controller
            APlayerController* controller = GetWorld()->GetFirstPlayerController();
            //Unposses current pawn and posses head pawn
            controller->UnPossess();
            controller->Possess(OriginalBody->EquippedHeadPawn);
            //Turn Control on for head pawn
            OriginalBody->EquippedHeadPawn->control = true;
        }
    }
}

//For Every tick
void ARightArmPawn::Tick(float DeltaTime)
{
    //Return to Body
    if (startReturn)
    {
        
        bool arriveLoc = ReturnLocation(DeltaTime);
        bool arriveRot = ReturnRotation(DeltaTime, arriveLoc);

        //If it has arrived to location
        if (arriveLoc)
        {
            //If it has arrive to rotation
            if (arriveRot)
            {
                //Control Body
                ControlBody();
                //Attach body part to body
                OriginalBody->Attach(this, 3);
                //Stop return sequence
                startReturn = false;
            }
        }
    }

    //Connect to Ability Character
    if (connectAbilityCharacter)
    {
        bool arriveLoc = ConnectLocation(DeltaTime);
        bool arriveRot = ConnectRotation(DeltaTime);

        //If arrived to location and rotation
        if (arriveLoc && arriveRot)
        {
            //Control Body
            ControlBody();
            //Attach body part to ability character
            LastAbility->Attach(this, 3);
            //stop connect ability sequence
            connectAbilityCharacter = false;
            //set ability pointer to null
            LastAbility = NULL;
        }
    }
}

//Player Input
void ARightArmPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

    Super::SetupPlayerInputComponent(PlayerInputComponent);

    //Connect to characters
    PlayerInputComponent->BindAction("Return", IE_Pressed, this, &ARightArmPawn::ConnectToBody);
    PlayerInputComponent->BindAction("RightArmOff", IE_Pressed, this, &ABodypartPawn::ConnectToAbilityCharacter);

    //Movement
    PlayerInputComponent->BindAxis("MoveForward", this, &ARightArmPawn::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ARightArmPawn::MoveRight);

    //Change Control
    PlayerInputComponent->BindAction("HeadOff", IE_Pressed, this, &ARightArmPawn::ControlHead);
    PlayerInputComponent->BindAction("LeftArmOff", IE_Pressed, this, &ARightArmPawn::ControlLeftArm);
}

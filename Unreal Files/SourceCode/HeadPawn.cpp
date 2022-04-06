// Fill out your copyright notice in the Description page of Project Settings.


#include "HeadPawn.h"
#include "BreakableCharacter.h"

//Constructor
AHeadPawn::AHeadPawn()
{
    //Set Up Mesh
     static ConstructorHelpers::FObjectFinder<UStaticMesh>HeadVisualAsset(TEXT("StaticMesh'/Game/Mannequin/Meshes/Head_Mesh.Head_Mesh'"));
     Mesh->SetStaticMesh(HeadVisualAsset.Object);
     Mesh->SetRelativeScale3D({ 0.15625,0.15625,0.15625 });
     //Equip Socket
     EquipSocket = FName("headSocket");
     //Damping
     Mesh->SetLinearDamping(2);
}

//Set Up Player Input
void AHeadPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    //Connect to characters
    PlayerInputComponent->BindAction("Return", IE_Pressed, this, &AHeadPawn::ConnectToBody);
    PlayerInputComponent->BindAction("HeadOff", IE_Pressed, this, &ABodypartPawn::ConnectToAbilityCharacter);

    //Movement
    PlayerInputComponent->BindAxis("MoveForward", this, &AHeadPawn::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AHeadPawn::MoveRight);

    //Control Different Body parts
    PlayerInputComponent->BindAction("LeftArmOff", IE_Pressed, this, &AHeadPawn::ControlLeftArm);
    PlayerInputComponent->BindAction("RightArmOff", IE_Pressed, this, &AHeadPawn::ControlRightArm);

}

//For Every tick
void AHeadPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //Return to original body
    if (startReturn)
    {
        bool arriveLoc = ReturnLocation(DeltaTime);
        bool arriveRot = ReturnRotation(DeltaTime, arriveLoc);

        //If body part arrived at body's location
        if (arriveLoc)
        {
            //If body part arrived at body's rotation
            if (arriveRot)
            {
                //Control Body
                ControlBody();
                //Attach this to its original body
                OriginalBody->Attach(this,1);
                //Stop return sequence
                startReturn = false;
            }
        }
    }

    //Connect to ability character
    if (connectAbilityCharacter)
    {
        bool arriveLoc = ConnectLocation(DeltaTime);
        bool arriveRot = ConnectRotation(DeltaTime);

        //If the vody parts rotation and location match the body's location and rotation
        if (arriveLoc && arriveLoc)
        {
            //Control the Ability Character
            ControlAbilityBody(LastAbility);
            //Attach this to the character
            LastAbility->Attach(this, 1);

            //Stop the connection sequence
            connectAbilityCharacter = false;
            //Set the last ability pointer to NUll 
            LastAbility = NULL;
        }
    }
}

//Return to original bodies rotation
bool AHeadPawn::ReturnRotation(float DeltaTime, bool arrived)
{
    //Get Sockets rotation
    FRotator bodyRotation = OriginalBody->GetMesh()->GetSocketRotation(EquipSocket);
    //Calculate rotation speed
    float rotateAmount = DeltaTime * moveToBodyRotationSpeed;

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

    //If Body part arrived at rotation
    if (arrived)
    {
        //Turn towards body's yaw
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
    }
    else
    {
        //Spin arround y-axis
        SetActorRotation({ GetActorRotation().Pitch ,GetActorRotation().Yaw + rotateAmount,GetActorRotation().Roll });
    }

    //Turn towards body's roll
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

    //If the rotation is the same as the body's rotation
    if (bodyRotation.Equals(GetActorRotation(), 1))
    {
        return true;
    }
    return false;
}

//Return to original bodies location
bool AHeadPawn::ReturnLocation(float DeltaTime)
{
    //Get the sockets location
    FVector bodyLocation = OriginalBody->GetMesh()->GetSocketLocation(EquipSocket);

    float highestPoint = 0;
    float movementSpeed = 0;

    //Caluclate hghest point and movement speed
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
        //If height is less than highest point
        if (GetActorLocation().Z < highestPoint)
        {
            //Move Upwards
            SetActorLocation({ GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + movementSpeed * DeltaTime });
        }
        else
        {
            //Teleport to body's xy-coordinates
            SetActorLocation({ bodyLocation.X, bodyLocation.Y,  GetActorLocation().Z + movementSpeed * DeltaTime });
        }
    }
    else
    {
        //If location is less than body's location
        if (GetActorLocation().Z <= bodyLocation.Z)
        {
            //Set to bodie's location
            SetActorLocation(bodyLocation);
        }
        else
        {
            //move towarsd negative z-axis
            SetActorLocation({ bodyLocation.X, bodyLocation.Y, GetActorLocation().Z - movementSpeed * DeltaTime });
        }
    }

    //If Body location is the same as body part location
    if (bodyLocation == GetActorLocation())
    {
        return true;
    }

    return false;
}

//Move Forward
void AHeadPawn::MoveForward(float AxisValue)
{
    //If this body part is controlled
    if (control)
    {
        //Get Rotation From Controller
        const FRotator Rotation = Controller->GetControlRotation();
        //Get Yaw Rotation
        const FRotator YawRotation(0, Rotation.Yaw + 90, 0);
        //Get Direction using the rotation and x-axis
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        //Add force towards that direction
        Mesh->AddForce({ (Direction.X) * AxisValue * movementSpeedX/2 ,(Direction.Y) * AxisValue * movementSpeedY/2 ,0 });
    }
}

//Move Right
void AHeadPawn::MoveRight(float AxisValue)
{
    //If this body part is controlled
    if (control)
    {
        //Get Rotation From Controller
        const FRotator Rotation = Controller->GetControlRotation();
        //Get Yaw Rotation
        const FRotator YawRotation(0, Rotation.Yaw + 90, 0);
        //Get Direction using the rotation and y-axis
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        //Add force towards that direction
        Mesh->AddForce({ (Direction.X) * AxisValue * movementSpeedX/2  ,(Direction.Y) * AxisValue * movementSpeedY/2 ,0 });
    }
}

//Change control to left arm
void AHeadPawn::ControlLeftArm()
{
    //If the start sequence is not active
    if (!startReturn)
    {
        //If a left arm exists and is unattached
        if (OriginalBody->EquippedLeftArmPawn && !OriginalBody->leftArmAttached)
        {
            //Turn off control for this body part
            control = false;
            //Get Controller
            APlayerController* controller = GetWorld()->GetFirstPlayerController();

            //Unposses this pawn and posses left arm pawn
            controller->UnPossess();
            controller->Possess(OriginalBody->EquippedLeftArmPawn);
            //Turn control on for that pawn
            OriginalBody->EquippedLeftArmPawn->control = true;
        }
    }
}

//Change control to right arm
void AHeadPawn::ControlRightArm()
{
    //If Start sequence is not running
    if (!startReturn)
    {
        //If right arm exists and is not attached
        if (OriginalBody->EquippedRightArmPawn && !OriginalBody->rightArmAttached)
        {
            //Turn off control for this pawn
            control = false;
            //Get Controller
            APlayerController* controller = GetWorld()->GetFirstPlayerController();
            //Unpossess this pawn and possess the right arm pawn
            controller->UnPossess();
            controller->Possess(OriginalBody->EquippedRightArmPawn);
            //Turn control for the right arm pawn to true
            OriginalBody->EquippedRightArmPawn->control = true;
        }
    }
}

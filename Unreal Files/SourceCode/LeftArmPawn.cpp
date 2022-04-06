 // Fill out your copyright notice in the Description page of Project Settings.


#include "LeftArmPawn.h"
#include "BreakableCharacter.h"

//Constructor
ALeftArmPawn::ALeftArmPawn()
{
    //Mesh Setup
    static ConstructorHelpers::FObjectFinder<UStaticMesh>LeftArmVisual(TEXT("StaticMesh'/Game/Mannequin/Meshes/LeftArm_Mesh.LeftArm_Mesh'"));
    Mesh->SetStaticMesh(LeftArmVisual.Object);
    Mesh->SetRelativeScale3D({ 0.01,0.01,0.01 });
    
    //Equip Socket
    EquipSocket = FName("leftArmSocket");
   
    //Damping
    Mesh->SetLinearDamping(1);
    
}

//Return to original body's rotation
bool ALeftArmPawn::ReturnRotation(float DeltaTime, bool arrived)
{
    //Get Sockets rotation
    FRotator bodyRotation = OriginalBody->GetMesh()->GetSocketRotation(EquipSocket);
    //Calculate rotate speed
    float rotateAmount = DeltaTime * moveToBodyRotationSpeed;

    //If location is arrived to body's location
    if (arrived)
    {
        //Turn towards correct Pitch
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

        //Turn towards correct Yaw
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

        //Turn towards correct Roll
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
        //Rotate around the y-axis
        SetActorRotation({ GetActorRotation().Pitch ,GetActorRotation().Yaw+ rotateAmount ,GetActorRotation().Roll });
    }

    //If the body part rotation matches the bodies rotation
    if (bodyRotation.Equals(GetActorRotation(), 1))
    {
        return true;
    }
    return false;
}

//Return to original body's location
bool ALeftArmPawn::ReturnLocation(float DeltaTime)
{
    //Get socket location
    FVector bodyLocation = OriginalBody->GetMesh()->GetSocketLocation(EquipSocket);

    float highestPoint = 0;
    float movementSpeed = 0;

    //Calculate highest point and moement speed
    if (bodyLocation.Z > returnPos.Z)
    {
        highestPoint = bodyLocation.Z + teleportingHeight;
        movementSpeed = (highestPoint - returnPos.Z) ;
    }
    else
    {
        highestPoint = returnPos.Z + teleportingHeight;
        movementSpeed = (highestPoint - bodyLocation.Z);
    }

    //If body part did not teleport
    if (!(GetActorLocation().X == bodyLocation.X) || !(GetActorLocation().Y == bodyLocation.Y))
    {
        //If body part is lower than the highest point
        if (GetActorLocation().Z < highestPoint)
        {
            //Move upwards
            SetActorLocation({ GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + movementSpeed * DeltaTime });
        }
        else
        {
            //Teleport to body's xy coordinates
            SetActorLocation({ bodyLocation.X, bodyLocation.Y,  GetActorLocation().Z + movementSpeed * DeltaTime });
        }
    }
    else
    {
        // if the bodypart is lower than the body's z coordinate
        if (GetActorLocation().Z <= bodyLocation.Z)
        {
            //Set to body's z coordinate
            SetActorLocation(bodyLocation);
        }
        else
        {
            //Move towards negative z-axis
            SetActorLocation({ bodyLocation.X, bodyLocation.Y, GetActorLocation().Z - movementSpeed * DeltaTime });
        }
    }

    //If the body location is the same as the body part's location
    if (bodyLocation == GetActorLocation())
    {
        return true;
    }




    return false;
}

//Move Forward
void ALeftArmPawn::MoveForward(float AxisValue)
{
    //If this body part is controlled
    if (control)
    {
        //Get Controller Rotation
        const FRotator Rotation = Controller->GetControlRotation();

        //Get The Yaw Rotation
        const FRotator YawRotation(0, Rotation.Yaw + 90, 0);

        //Get Direction using rotation and x-axis
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        //Add force towards that direction
        Mesh->AddForce({ (Direction.X) * AxisValue * movementSpeedX ,(Direction.Y) * AxisValue * movementSpeedX ,0 });
    }
}

//Move Right
void ALeftArmPawn::MoveRight(float AxisValue)
{
    //If the body part is controlled
    if (control)
    {
        //Get Controller Rotation
        const FRotator Rotation = Controller->GetControlRotation();

        //Get Yaw Rotation
        const FRotator YawRotation(0, Rotation.Yaw + 90, 0);

        //Get direction using rotation and y-axis
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        //Add force towards that direction
        Mesh->AddForce({ (Direction.X) * AxisValue * movementSpeedX  ,(Direction.Y) * AxisValue * movementSpeedY ,0 });
    }
}

//Set Up Input
void ALeftArmPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    //Connect to characters
    PlayerInputComponent->BindAction("Return", IE_Pressed, this, &ALeftArmPawn::ConnectToBody);
    PlayerInputComponent->BindAction("LeftArmOff", IE_Pressed, this, &ABodypartPawn::ConnectToAbilityCharacter);

    //Movement
    PlayerInputComponent->BindAxis("MoveForward", this, &ALeftArmPawn::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ALeftArmPawn::MoveRight);

    //Change Controlled body part
    PlayerInputComponent->BindAction("HeadOff", IE_Pressed, this, &ALeftArmPawn::ControlHead);
    PlayerInputComponent->BindAction("RightArmOff", IE_Pressed, this, &ALeftArmPawn::ControlRightArm);
}

//Controll Head
void ALeftArmPawn::ControlHead()
{
    //If the boody part is not currently returning to its original body
    if (!startReturn)
    {
        //If head exists and is not attached
        if (OriginalBody->EquippedHeadPawn && !OriginalBody->headAttached)
        {
            //Turn control for this body part off
            control = false;
            //Get Player controller
            APlayerController* controller = GetWorld()->GetFirstPlayerController();
          
            //Unpossess this pawn and posses Head pawn
            controller->UnPossess();
            controller->Possess(OriginalBody->EquippedHeadPawn);
           
            //Turn control on for head pawn
            OriginalBody->EquippedHeadPawn->control = true;
        }
    }
}

//Control Right Arm
void ALeftArmPawn::ControlRightArm()
{    
    //If the boody part is not currently returning to its original body
    if (!startReturn)
    {
        //If right arm exists and is not attached
        if (OriginalBody->EquippedRightArmPawn && !OriginalBody->rightArmAttached)
        {
            //Turn control for this body part off
            control = false;
            //Get PLayer Controller
            APlayerController* controller = GetWorld()->GetFirstPlayerController();

            //Unpossess this pawn and possess Right Arm pawn
            controller->UnPossess();
            controller->Possess(OriginalBody->EquippedRightArmPawn);

            //Turn control on for right arm pawn
            OriginalBody->EquippedRightArmPawn->control = true;
        }
    }
}

//For Every frame
void ALeftArmPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //Return to Body
    if (startReturn)
    {
        bool arriveLoc = ReturnLocation(DeltaTime);
        bool arriveRot = ReturnRotation(DeltaTime, arriveLoc);

        //If the body part reaches bodies location
        if (arriveLoc)
        {
            //If the body part rotation reaches bodies rotation
            if (arriveRot)
            {
                //Control Body
                ControlBody();
                //Attach this pawn to original body
                OriginalBody->Attach(this, 2);
                //Stop return sequence
                startReturn = false;
            }
        }
    }

    //Connect to an Ability Character
    if (connectAbilityCharacter)
    {
        bool arriveLoc = ConnectLocation(DeltaTime);
        bool arriveRot = ConnectRotation(DeltaTime);

        //If both location and rotation reached the bodies location and rotation
        if (arriveLoc && arriveRot)
        {
            //Control Body
            ControlBody();
            //Attach this pawn to the Ability character
            LastAbility->Attach(this, 2);

            //Stop connect to ability character sequence
            connectAbilityCharacter = false;
            //Last ability pointer becomes NUll
            LastAbility = NULL;
        }
    }
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityCharacter.h"

//Constructor
AAbilityCharacter::AAbilityCharacter()
{
	//Setup Widget
	widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	widget->SetupAttachment(GetMesh());
	
	//Setup Collision Sphere
	sphereReach = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	sphereReach->SetSphereRadius(200);
	sphereReach->SetupAttachment(GetMesh());
	sphereReach->SetGenerateOverlapEvents(true);

	//Set up attached
	headAttached = false;
	leftArmAttached = false;
	rightArmAttached = false;
	controlBody = false;
}

void AAbilityCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Set Widget Visibility
	widget->SetVisibility(false);
}

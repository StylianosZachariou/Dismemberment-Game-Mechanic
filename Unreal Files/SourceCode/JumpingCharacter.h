// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityCharacter.h"
#include "JumpingCharacter.generated.h"


UCLASS()
class CMP302_API AJumpingCharacter : public AAbilityCharacter
{
	GENERATED_BODY()
public:
	//Constructor
	AJumpingCharacter();

protected:
	//Overlap Functions
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

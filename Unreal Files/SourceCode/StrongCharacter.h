// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityCharacter.h"
#include "StrongCharacter.generated.h"


UCLASS()
class CMP302_API AStrongCharacter : public AAbilityCharacter
{
	GENERATED_BODY()
	
public:
	//Contructor
	AStrongCharacter();

	//Changed By the moveable object, true if character is pushing
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool isPushing;

	//Left Arm Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = Widget)
		UWidgetComponent* lawidget;

	//Right Arm Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget)
		UWidgetComponent* rawidget;

	//True if both arms attached
	bool canPush;

protected:

	//Overlap Functions
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Detach all body parts
	void removeAll();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
};

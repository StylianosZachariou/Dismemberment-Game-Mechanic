// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/BoxComponent.h"
#include "StrongCharacter.h"

#include "MovableObject.generated.h"

UCLASS()
class CMP302_API AMovableObject : public AActor
{
	GENERATED_BODY()

public:	
	//Constructor
	AMovableObject();
	
	//Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
		UStaticMeshComponent* Mesh;

	//Physics Constraints
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics)
		UPhysicsConstraintComponent* PhysicsConstraint;

	//Collision boox
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics)
		UBoxComponent* Box;

protected:
	//Overlap functions
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Character Pushing
	AStrongCharacter* character;

	//Material
	UMaterial* material;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

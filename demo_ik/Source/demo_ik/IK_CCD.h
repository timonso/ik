// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "APosableCharacter.h"
#include "Components/PoseableMeshComponent.h" 

#include "IK_CCD.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEMO_IK_API UIK_CCD : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIK_CCD();

	UPROPERTY(EditAnywhere)
	class AActor* targetActor_reference;

	AAPosableCharacter* PosableCharacter;
	UPoseableMeshComponent* PosableMesh;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void Solve(UPoseableMeshComponent* skeleton, const FVector &targetPosition, TArray<FString> &boneVectors, float threshold = 0.01, int iterationCount = 10);
};

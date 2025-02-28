// Fill out your copyright notice in the Description page of Project Settings.

#include "IK_Fabrik.h"
#include "Math/UnrealMathVectorCommon.h"


// Sets default values for this component's properties
UIK_Fabrik::UIK_Fabrik()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	targetActor_reference = CreateDefaultSubobject<AActor>(TEXT("Actor"));
	PosableCharacter = Cast<AAPosableCharacter>(GetOwner());
}

// FABRIK solver
void UIK_Fabrik::Solve(UPoseableMeshComponent* skeleton, const FVector& targetPosition, TArray<FString>& boneNames, float threshold, int iterationCount)
{
	// check if the bone vectors are empty
	if (boneNames.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("no bone vectors found for FABRIK"));
		return;
	}


	// iteratively approximate a solution
	for (int i = 0; i < iterationCount; i++)
	{
		for (int b = 0; b < boneNames.Num(); b++) {
			FVector endBonePos = skeleton->GetBoneLocation(FName(boneNames[0],  EBoneSpaces::WorldSpace));
			if (FVector::Dist(endBonePos, targetPosition) < threshold) {
				return;
			}
			FName currentBoneName = FName(boneNames[b]);
			FVector currentBonePos = skeleton->GetBoneLocation(currentBoneName, EBoneSpaces::WorldSpace);
			FQuat currentBoneRot = skeleton->GetBoneQuaternion(currentBoneName, EBoneSpaces::WorldSpace);
			FVector targetDirection = (targetPosition - currentBonePos);
			FVector endBoneDirection = (endBonePos - currentBonePos);
			FQuat newBoneRot = FQuat::FindBetweenVectors(endBoneDirection, targetDirection) * currentBoneRot;
			skeleton->SetBoneRotationByName(currentBoneName, newBoneRot.Rotator(), EBoneSpaces::WorldSpace);
		}

	}
}

// Called when the game starts
void UIK_Fabrik::BeginPlay()
{
	Super::BeginPlay();

	if (PosableCharacter) {
		PosableMesh = PosableCharacter->posableMeshComponent_reference;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Fabrik: Poseable character not found"));
	}
}

// Called every frame
void UIK_Fabrik::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<FString> boneNames = {
			TEXT("hand_l"),
			TEXT("lowerarm_l"),
			TEXT("upperarm_l"),
	};

	TArray<BoneVector> boneVectors = {};

	for (int i = 0; i < boneNames.Num(); i++)
	{
		BoneVector boneVector = BoneVector();
		boneVector.name = boneNames[i];
		boneVector.position = PosableMesh->GetBoneLocation(FName(boneNames[i]), EBoneSpaces::WorldSpace);
		boneVector.rotation = PosableMesh->GetBoneQuaternion(FName(boneNames[i]), EBoneSpaces::WorldSpace);
		boneVectors.Add(boneVector);
	}

	Solve(PosableMesh, targetActor_reference->GetActorLocation(), boneNames);
}

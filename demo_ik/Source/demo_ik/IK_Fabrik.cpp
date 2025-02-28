// Fill out your copyright notice in the Description page of Project Settings.

#include "IK_Fabrik.h"


// Sets default values for this component's properties
UIK_Fabrik::UIK_Fabrik()
{
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
		UE_LOG(LogTemp, Warning, TEXT("no bone vectors found for CCD"));
		return;
	}

	// iteratively approximate a solution
	for (int i = 0; i < iterationCount; i++)
	{
		for (int b = 0; b < boneNames.Num(); b++) {
			// end effector
			FVector endBonePos = skeleton->GetBoneLocation(FName(boneNames[0],  EBoneSpaces::WorldSpace));

			// check if the end effector is close enough to the target
			if (FVector::Dist(endBonePos, targetPosition) < threshold) {
				return;
			}

			FName currentBoneName = FName(boneNames[b]);
			FVector currentBonePos = skeleton->GetBoneLocation(currentBoneName, EBoneSpaces::WorldSpace);
			FQuat currentBoneRot = skeleton->GetBoneQuaternion(currentBoneName, EBoneSpaces::WorldSpace);
			FVector targetDirection = (targetPosition - currentBonePos);
			FVector endBoneDirection = (endBonePos - currentBonePos);

			// determine and apply the appropriate rotation towards the target
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
		UE_LOG(LogTemp, Warning, TEXT("CCD: Poseable character not found"));
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

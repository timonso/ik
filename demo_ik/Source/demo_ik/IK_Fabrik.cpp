// Fill out your copyright notice in the Description page of Project Settings.

#include "IK_Fabrik.h"

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
void UIK_Fabrik::Solve(const FVector &targetPosition, TArray<BoneVector> &boneVectors, float threshold, int iterationCount)
{
	// check if the bone vectors are empty
	if (boneVectors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("no bone vectors found for FABRIK"));
		return;
	}

	FVector startBone = boneVectors[0].position;

	// iteratively approximate a solution
	for (int i = 0; i < iterationCount; i++)
	{
		// check if the last bone is close enough to the target position
		FVector endBone = boneVectors.Last().position;
		if (FVector::Distance(endBone, targetPosition) <= threshold)
		{
			break;
		}

		// backward pass
		 boneVectors.Last().position = targetPosition;
		 for (int b = boneVectors.Num() - 2; b >= 0; b--)
		 {
			 FVector Direction = (boneVectors[b].position - boneVectors[b + 1].position).GetSafeNormal();
			 float BoneLength = FVector::Distance(boneVectors[b].position, boneVectors[b + 1].position);
			 boneVectors[b].position = boneVectors[b + 1].position + Direction * BoneLength;
		 }

		 // forward pass
		 boneVectors[0].position = startBone;
		 for (int f = 1; f < boneVectors.Num(); f++)
		 {
			 FVector Direction = (boneVectors[f].position - boneVectors[f - 1].position).GetSafeNormal();
			 float BoneLength = FVector::Distance(boneVectors[f].position, boneVectors[f - 1].position);
			 boneVectors[f].position = boneVectors[f - 1].position + Direction * BoneLength;
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
void UIK_Fabrik::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<FString> boneNames = {
			TEXT("upperarm_l"),
			TEXT("lowerarm_l"),
			TEXT("hand_l")};

	TArray<BoneVector> boneVectors = {};

	for (int i = 0; i < boneNames.Num(); i++)
	{
		BoneVector boneVector = BoneVector();
		boneVector.name = boneNames[i];
		boneVector.position = PosableMesh->GetBoneLocationByName(FName(boneNames[i]), EBoneSpaces::WorldSpace);
		boneVector.magnitude = 0.0f;
		boneVectors.Add(boneVector);
	}

	Solve(targetActor_reference->GetActorLocation(), boneVectors);

	for (int i = 0; i < boneVectors.Num(); i++)
	{
		PosableMesh->SetBoneLocationByName(FName(boneVectors[i].name), boneVectors[i].position, EBoneSpaces::WorldSpace);
	}
}

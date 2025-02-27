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
	PosableMesh = PosableCharacter->posableMeshComponent_reference;
}

void UIK_Fabrik::Solve(const FVector &targetPosition, const TArray<FVector> &boneVectors, float threshold, int iterationCount)
{

}

// Called when the game starts
void UIK_Fabrik::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UIK_Fabrik::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<FVector> boneVectors = {
		PosableMesh->GetBoneLocationByName(FName("upperarm_l"), EBoneSpaces::WorldSpace),
		PosableMesh->GetBoneLocationByName(FName("lowerarm_l"), EBoneSpaces::WorldSpace),
		PosableMesh->GetBoneLocationByName(FName("hand_l"), EBoneSpaces::WorldSpace)
	};

	Solve(targetActor_reference->GetActorLocation(), boneVectors);

	PosableMesh->SetBoneLocationByName(FName("upperarm_l"), EBoneSpaces::WorldSpace),
	PosableMesh->SetBoneLocationByName(FName("lowerarm_l"), EBoneSpaces::WorldSpace),
	PosableMesh->SetBoneLocationByName(FName("hand_l"), EBoneSpaces::WorldSpace)
}

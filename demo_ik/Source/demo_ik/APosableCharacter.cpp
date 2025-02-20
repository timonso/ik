// Fill out your copyright notice in the Description page of Project Settings.

#include "APosableCharacter.h"

// Sets default values
AAPosableCharacter::AAPosableCharacter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// create the component instance to attach to the actor (this is required)
	posableMeshComponent_reference = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PoseableMesh"));

	// attach your posable mesh component to the root (make it a child of the root component)
	posableMeshComponent_reference->SetMobility(EComponentMobility::Movable);
	posableMeshComponent_reference->SetVisibility(true);
	posableMeshComponent_reference->SetupAttachment(RootComponent);
	RootComponent = posableMeshComponent_reference;
	
	// reference the mannequin asset by path (be aware to check the path if this is not working). 
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MannequinMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple"));	
	if (MannequinMesh.Succeeded())
	{
		// set default mesh with the mannequin access and apply it to the posable mesh.
		default_skeletalMesh_reference = MannequinMesh.Object;
		posableMeshComponent_reference->SetSkinnedAssetAndUpdate(default_skeletalMesh_reference);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("manequin not found, check the path."));
	}

	// create the component instance to attach to the actor (this is required)
	targetSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("targetSphere"));

	// define the properties of the target sphere and attach it to the root.
	targetSphere->SetMobility(EComponentMobility::Movable);
	targetSphere->SetVisibility(true);
	targetSphere->SetupAttachment(RootComponent);

	// reference the sphere asset by path (be aware to check the path if this is not working).
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
	if (sphereMesh.Succeeded())
	{
		// set the mesh with the source sphere asset.
		targetSphereAsset = sphereMesh.Object;
		targetSphere->SetStaticMesh(targetSphereAsset);
		// set the scaling of the sphere.
		targetSphere->SetWorldScale3D(FVector(targetSphereScaling, targetSphereScaling, targetSphereScaling));
		
		// reference the sphere material by path (be aware to check the path if this is not working).
		static ConstructorHelpers::FObjectFinder<UMaterialInterface> sphereMaterial(TEXT("/Game/material/M_targetSphere"));
		if (sphereMaterial.Succeeded())
		{
			// set the material with the source material asset.
			targetSphereMaterial = UMaterialInstanceDynamic::Create(sphereMaterial.Object, this, FName("sphereTarget_dynamicMaterial"));
			if (targetSphereMaterial)
			{
				targetSphere->SetMaterial(0, targetSphereMaterial);
				// set the color of the material with the provided color.
				targetSphereMaterial->SetVectorParameterValue(TEXT("Color"), targetSphereColor);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("sphere not found, check the path."));
	}

}

bool AAPosableCharacter::initializePosableMesh()
{
	// initialization checks to avoid crashes.
	if (!posableMeshComponent_reference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Posable mesh component not attached or registerd"));
		return false;
	}
	if (!default_skeletalMesh_reference)
	{
		UE_LOG(LogTemp, Warning, TEXT("No skeletal mesh reference provided."));
		return false;
	}

	// set up the poseable mesh component.
	posableMeshComponent_reference->SetSkinnedAssetAndUpdate(default_skeletalMesh_reference);
	return true;
}

void AAPosableCharacter::waving_playStop()
{
	session1_isPlaying = !session1_isPlaying;
}

void AAPosableCharacter::testSetTargetSphereRelativePosition()
{
	setTargetSphereRelativePosition(targetSphereTestRelativePosition);
}

void AAPosableCharacter::setTargetSphereRelativePosition(FVector newPosition)
{
	if (targetSphere)
	{
		targetSphere->SetRelativeLocation(newPosition);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("target sphere not found!"));
	}
}

bool AAPosableCharacter::doesBoneOrSocketNameExists(FName inputName)
{
	// initialization check to avoid crashes.
	if (!posableMeshComponent_reference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Posable mesh component not attached or registerd"));
		return false;
	}
	// check over all the bones names
	TArray<FName> _boneNames = TArray<FName>();
	posableMeshComponent_reference->GetBoneNames(_boneNames);
	return(
		_boneNames.Contains(inputName)
		or posableMeshComponent_reference->DoesSocketExist(inputName)
	);
}

void AAPosableCharacter::setVisibility(bool visible)
{
	// initialization check to avoid crashes.
	if (!posableMeshComponent_reference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Posable mesh component not attached or registerd"));
	}
	else
		posableMeshComponent_reference->SetVisibility(visible);
}

void AAPosableCharacter::storeCurrentPoseRotations(TArray<FRotator>& storedPose)
{
	// initialization check to avoid crashes.
	if (!posableMeshComponent_reference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Posable mesh component not attached or registerd"));
		return;
	}

	const int32 NumBones = posableMeshComponent_reference->GetNumBones();
	storedPose.SetNum(NumBones);

	for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
	{
		FName BoneName = posableMeshComponent_reference->GetBoneName(BoneIndex);
		if (BoneName != NAME_None)
		{
			storedPose[BoneIndex] = posableMeshComponent_reference->GetBoneRotationByName(BoneName, EBoneSpaces::ComponentSpace);
		}
	}
}

void AAPosableCharacter::waving_initializeStartingPose()
{
	// initialization check to avoid crashes.
	if (!posableMeshComponent_reference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Posable mesh component not attached or registerd"));
		return;
	}

	// get time (for the periodic function)
	const float Time = GetWorld()->GetTimeSeconds();

	// we declare the bones we want to edit.
	FName upperArmBoneName = FName("upperarm_r");
	FName clavicleBoneName = FName("clavicle_r");

	// upperarm
	if (doesBoneOrSocketNameExists(upperArmBoneName))
	{
		
		// (2) get the bone transform in component space.
		FTransform bone_componentSpaceTransform  = posableMeshComponent_reference->GetBoneTransformByName(
			upperArmBoneName,
			EBoneSpaces::ComponentSpace
		);
		
		// (3) get the bone's parent transform in component space.
		FTransform parent_componentSpaceTransform = posableMeshComponent_reference->GetBoneTransformByName(
			posableMeshComponent_reference->GetParentBone(upperArmBoneName),
			EBoneSpaces::ComponentSpace
		);
		
		// (4) extract the bone transform relative to the parent reference system (using FTransform built in function). 
		FTransform bone_relativeTransform  = bone_componentSpaceTransform .GetRelativeTransform(parent_componentSpaceTransform);
		
		// (5) define the relative rotation we want to set.
		FRotator relativeBoneRotation = FRotator(21.435965, 21.709806, -92.235083);
		//(Pitch = 21.435965, Yaw = 21.709806, Roll = -92.235083)


		// (6) set the rotation in the relative transform. (note that transform use quaternions internally, so we need to use the built in function to extract the quaternion from the Rotator).
		bone_relativeTransform .SetRotation(relativeBoneRotation.Quaternion());

		// (7) change back from relative space to component space. 
		FTransform newBoneTransform_world = bone_relativeTransform  * parent_componentSpaceTransform;
		
		// (8) finally we set the rotation, in component space, by name.
		posableMeshComponent_reference->SetBoneRotationByName(
			upperArmBoneName,
			newBoneTransform_world.Rotator(),
			EBoneSpaces::ComponentSpace
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("bone: %s not found!"), *upperArmBoneName.ToString());
	}

	if (doesBoneOrSocketNameExists(clavicleBoneName))
	{
		FTransform bone_componentSpaceTransform  = posableMeshComponent_reference->GetBoneTransformByName(
			clavicleBoneName,
			EBoneSpaces::ComponentSpace
		);
		FTransform parent_componentSpaceTransform = posableMeshComponent_reference->GetBoneTransformByName(
			posableMeshComponent_reference->GetParentBone(clavicleBoneName),
			EBoneSpaces::ComponentSpace
		);
		FTransform bone_relativeTransform  = bone_componentSpaceTransform .GetRelativeTransform(parent_componentSpaceTransform);
		
		FRotator relativeBoneRotation = FRotator(-78.486128, 177.309228, 13.290207);
		
		bone_relativeTransform .SetRotation(relativeBoneRotation.Quaternion());
		FTransform newBoneTransform_world = bone_relativeTransform  * parent_componentSpaceTransform;
		posableMeshComponent_reference->SetBoneRotationByName(
			clavicleBoneName,
			newBoneTransform_world.Rotator(),
			EBoneSpaces::ComponentSpace
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("bone: %s not found!"), *clavicleBoneName.ToString());
	}

	// Update transforms to ensure the changes are applied.
	posableMeshComponent_reference->RefreshBoneTransforms();
}

void AAPosableCharacter::waving_tickAnimation()
{
	// initialization check to avoid crashes.
	if (!posableMeshComponent_reference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Posable mesh component not attached or registerd"));
		return;
	}
	const float currentTime = GetWorld()->GetTimeSeconds();
	const int32 numBones = posableMeshComponent_reference->GetNumBones();
	if (waving_initialBoneRotations.Num() != numBones)
	{
		UE_LOG(LogTemp, Warning, TEXT("You need to call session1_initialBoneRotations first!"));
		return;
	}

	FName lowerarmBoneName = FName("lowerarm_r");

	if (doesBoneOrSocketNameExists(lowerarmBoneName))
	{
		FTransform bone_componentSpaceTransform  = posableMeshComponent_reference->GetBoneTransformByName(
			lowerarmBoneName,
			EBoneSpaces::ComponentSpace
		);
		int currentBoneIndex = posableMeshComponent_reference->GetBoneIndex(lowerarmBoneName);
		FRotator storedRotation = waving_initialBoneRotations[currentBoneIndex];
		bone_componentSpaceTransform .SetRotation(storedRotation.Quaternion());
			
		FTransform parent_componentSpaceTransform = posableMeshComponent_reference->GetBoneTransformByName(
			posableMeshComponent_reference->GetParentBone(lowerarmBoneName),
			EBoneSpaces::ComponentSpace
		);
		FTransform bone_relativeTransform  = bone_componentSpaceTransform .GetRelativeTransform(parent_componentSpaceTransform);
			
		// calculate the rotation offset angle using a sine wave function
		float angleOffset = FMath::Sin(waving_animationSpeed * currentTime) * waving_amplitude; // 30 degrees amplitude
		FRotator rotationOffset(0.0f, angleOffset, 0.0f);

		// apply the offset to the initial rotation
		FRotator relativeBoneRotation = bone_relativeTransform .Rotator() + rotationOffset;

		bone_relativeTransform .SetRotation(relativeBoneRotation.Quaternion());

		FTransform newBoneTransform_world = bone_relativeTransform  * parent_componentSpaceTransform;
		posableMeshComponent_reference->SetBoneRotationByName(
			lowerarmBoneName,
			newBoneTransform_world.Rotator(),
			EBoneSpaces::ComponentSpace
		);

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("bone: %s not found!"), *lowerarmBoneName.ToString());
	}

}

// Called when the game starts or when spawned
void AAPosableCharacter::BeginPlay()
{
	Super::BeginPlay();
	initializePosableMesh();
	waving_initializeStartingPose();
	waving_initialBoneRotations = TArray<FRotator>();
	storeCurrentPoseRotations(waving_initialBoneRotations);	
}

// Called every frame
void AAPosableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(session1_isPlaying)
		waving_tickAnimation();

}


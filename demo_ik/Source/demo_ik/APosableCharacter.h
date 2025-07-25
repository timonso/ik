// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PoseableMeshComponent.h" 
#include "APosableCharacter.generated.h"


/**
 * It is a skeletal mesh which pose can be modify directly on the main thread (posable mesh).
 * it has to be initialized starting from a source skeletal mesh component (which can only be modify on the animation thread).
 * It is used to display modifications such as IK.
 */

UCLASS()
class DEMO_IK_API AAPosableCharacter : public AActor
{
	GENERATED_BODY()

public:
	/**
	* the poseable mesh component.
	**/
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UPoseableMeshComponent* posableMeshComponent_reference;
	/**
	* the default skeletal mesh component.
	**/
	UPROPERTY(EditAnywhere, Category = Mesh) 
	USkeletalMesh* default_skeletalMesh_reference;
	/**
	* the waving animation speed.
	**/
	UPROPERTY(EditAnywhere, Category = "waving animation")
	float waving_animationSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "hand-to-heart animation")
	float handToHeart_animationSpeed = 1.0f;

	bool session1_isPlaying = false;

	UPROPERTY(EditAnywhere, Category = "hand-to-heart animation")
	bool handToHeart_isPlaying = false;

	/**
	* the waving animation amplitude.
	**/
	UPROPERTY(EditAnywhere, Category = "waving animation")
	float waving_amplitude = 30.0f;


protected:
	/**
	* the set of initial bone rotations for the waving animation, after setting the starting pose.
	**/
	TArray<FRotator> initialBoneRotations;


public:	
	/**
	* constructor.
	**/
	AAPosableCharacter();

	/**
	* simplified function for student example, without parameters.
	* @return: true if the initialization was successful, false otherwise
	**/
	bool initializePosableMesh();

	/**
	* function to play or stop the waving animation.
	* UPROPERTY is used to allow the function to be called from the editor.
	**/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "waving animation")
	void waving_playStop();


	/**
	* check if a Bone or Socket name exists.
	* @param inputName: the name of the bone or socket.
	* @return: true if it exists, false otherwise.
	**/
	bool doesBoneOrSocketNameExists(FName inputName);
	
	/**
	* set the of the mesh.
	* @param visible: true to make the mesh visible, false to hide it.
	**/
	void setVisibility(bool visible);


protected:
	/**
	* the initial bone rotations for the waving animation.
	**/
	void storeCurrentPoseRotations(TArray<FRotator>& storedPose);
	/**
	* waving animation initialization (used ib BeginPlay).
	**/
	void waving_initializeStartingPose();

	/**
	* waving animation tick (used in Tick).
	**/
	void waving_tickAnimation();

	void handToHeart_tickAnimation();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

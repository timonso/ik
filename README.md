## demo project.
#### project setup:
[tutorial for setting up visual studio with unreal](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine)
launch the 5.5 version of Unreal from the Epic Games Launcher.

![alt text](images/01-unrealEditor-two.png "unrealEngineVersion_ik")

create a new game.select `Games` and then select the `third person template`, just to get the default Unreal's character skeletal mesh (note that `first person template` just load a model with the arms and hands only). Additionally under `project details` select `C++`, since we want to create our ik system through code and not in blueprints. Optionally include the `starter content` for additional assets.
![alt text](images/02-unrealEditor.png "unrealEngineProjectCreation_ik")
#### crete a c++ class:
once the project is created the editor automatically open. under `tools` select `new c++ class...`, this tool helps creating c++ class formatted for Unreal. 
![alt text](images/03-newCplusplusFile.png "unrealNewc++Class_ik")
in our case we need to create an `actor` because we want the create a mesh that can be spawned and placed in the world.
![alt text](images/03-newCplusplusFile-actor.png "unrealNewActor_ik")
chose a name for your actor, in the example we set `APosableCharacter`, note that Unreal's naming convention guidelines suggest to call actor with the `A` prefix, this is not mandatory.
![alt text](images/03-newCplusplusFile-namingConvention.png "unrealNewActorNaming_ik")
Note: a **posable mesh** in Unreal Engine is a specific mesh that can be edited directly through c++ code, without needing to pass by the animation system. We will use this to implement our IK code. Don't confuse it with the **skeletal mesh** which is the default class of mesh with a skeleton, and that can be animated through animation blueprints, but not in c++. We will use this skeletal mesh as input to create the posable one.
#### setting up a posable mesh:
The previous steps should create a default class derived from AActor base classes, organized as header (`.h`) and `.cpp` file. this class comes with a couple of predefined functions (constructor, beginPlay, tick).  So your `.h` file should look more or less like this:
```cpp
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "APosableCharacter.generated.h"

UCLASS()
class DEMO_IK_API APosableCharacter : public AActor
{
    GENERATED_BODY()
    
public:    
    // Sets default values for this actor's properties
    APosableCharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:    
    // Called every frame
    virtual void Tick(float DeltaTime) override;

};
```
the first step is to get access to the posable mesh functionalities by include the necessary dependencies: `#include "Components/PoseableMeshComponent.h"`, note that you should always keep the `#include "<filename>.generated.h"` as last dependency of the list.  This allows us to declare a posable mesh component, here the declaration in the `.h` file:
```cpp
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UPoseableMeshComponent* posableMeshComponent_reference;
```
this can be defined either `public`, `protected` or eventually `private`, depends on the intended usage. 
Additionally we need to define a source skeletal mesh (this is the standard type of any skeleton-based character in Unreal). Practically this represent the character we want to use and edit with our posable mesh. So, still in the `.h` file we add this:
```cpp
public:
	UPROPERTY(EditAnywhere, Category = Mesh) 
	USkeletalMesh* default_skeletalMesh_reference;
```
ideally we want this to be `public` and with `UPROPERTY` set to `EditAnywhere`, so we can pick and select the character we want from the editor interface.

Let's now attach the posable mesh component to our actor (note that actor components are modular property that we can add to actors, and the posable mesh component is one of those). We do this using the predefined constructor. In your `.cpp` file, inside the constructor function you need to instantiate the posable mesh component using the `CreateDefaultSubobject` and then define the attachment to the actor, by either attach it to the actor `RootComponent` or by set the posable mesh component as `RootComponent`. Here an example:
```cpp
AAPosableCharacter::AAPosableCharacter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
	posableMeshComponent_reference = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PoseableMesh"));
	posableMeshComponent_reference->SetMobility(EComponentMobility::Movable);
	RootComponent = posableMeshComponent_reference;
}
```
Now, the only missing step is to initialize the posable mesh, using the given input skeletal mesh. To do so, we create a function, I called it `initializePosableMesh`, so that the declaration in the `.h` file would look like this:
```cpp
bool initializePosableMesh();
```
And the correspondent implementation in `.cpp` like this:
```cpp
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
	// create the poseable mesh component.
	posableMeshComponent_reference->SetSkinnedAssetAndUpdate(default_skeletalMesh_reference);
	return true;
}
```
Finally we need to call this function somewhere. In fact, while the constructor is automatically called at the generation of the scene, custom-function need to be called specifically. Therefore, we include this into the predefined `BeginPlay()` function in the `.cpp` file. Like this:
```cpp
// Called when the game starts or when spawned
void AAPosableCharacter::BeginPlay()
{
	Super::BeginPlay();
	initializePosableMesh();
}
```
The `BeginPlay()` function is called automatically by the engine when we play the game, this means that our initialization will take place only after, and so the mesh will be visible only in play mode. Of course, here we just show one possible approach, but, if you prefer, you can refer the source skeletal mesh directly in c++, and edit the code so it runs directly in the constructor.
#### adding our actor to the scene:
Now that the actor is defined in c++ we need to compile the code. Note that you need to compile the c++ code every time you modify something, to see the effect on the editor. There are different way to manage and compile `c++` code. Unreal offers a live compiling solution (without the need to reboot the system), you can find it in the bottom right of the editor interface:
![alt text](images/04-compile.png "unrealNewActorCompiling_ik")
Once the code is compiled you will be able to find your actor in the content browser. If you don't, be sure that `Show C++ Classes ` is ticked in the component browser settings.
![alt text](images/05-newCplusplus-cplusplus.png "unrealNewActorCompiling_cpp")
So now you can find all the `c++` files under `C++ Classes` folder. And if the compilation was successful you will be able to see our `APosableCharacter` file.
![alt text](images/06-newCplusplus-findMyActor.png "unrealNewActor_find")
To include this in the scene, you just need to drag and drop the file in the scene. To check if it was correctly placed, you can check the scene hierarchy on the right. You should see the actor there. If it does not appear, it might be that you have some compilation problems.
![alt text](images/07-newCplusplus-findMyActorInTheScene.png "unrealNewActor_findInScene")
As you can see it is normal that nothing appear in the scene, because, as we saw before, our mesh is initialized only in play mode, and we haven't select the source mesh yet. In face if we press play now, we will just see the player of the Unreal default `Third person` character, that is not our posable mesh.
![alt text](images/08-newCplusplus-play.png "unrealNewActor_play")
![alt text](images/08-newCplusplus-playCharacter.png "unrealNewActor_wrongCharacter")
The only missing step is to select the mesh we want to use as input to generate our posable character. Given that we expose the variable using `UPROPERTY`, we are able to easily select our input mesh in the editor, by first selecting the `APosableCharacter` in the hierarchy and then by clicking on the `Details panel`, under the `Mesh` category the property `Default Skeletal Mesh Reference`. Unreal will automatically show us the compatible assets. (Note, do not edit this in game mode, otherwise the changes would not be saved). 
![alt text](images/08-newCplusplus-editCharacter.png "unrealNewActor_setInput")
In this example I have selected the old UnrealEngine4 `SK_Mannequin` skeletal mesh, that, if you started with the `Third person` project you should find them following folder: `/Game/Characters/Mannequin_UE4/Meshes`. But you can use any skeletal mesh you like (try to load the new UE5 mannequin male or female). 
Once selected if we press play again, we should be able to see our posable mesh character in the default A-pose, next to the third person player.
![alt text](images/09-newCplusplus-characters.png "unrealNewActor_result1")
If you don't want to see the third person player every time you play the scene, you should edit the game mode. Or you can press `simulate` instead of play.
![alt text](images/09-newCplusplus-simulate.png "unrealNewActor_simulate")
![alt text](images/09-newCplusplus-simulateResult.png "unrealNewActor_result2")
#### access and edit posable mesh bones in c++:
Now our system is properly working, and we can set up the code to edit and access each individual bone in the hierarchy. In fact, the posable mesh component allows to access and modify bones properties in the main thread (`BeginPlay` and `Tick` functions). We can easily find the functions templates in the documentation or by directly looking at class and parents'classes definitions, here for example, some relevant function declared in `PoseableMeshComponent.h`:
```cpp
[...]
	UFUNCTION(BlueprintCallable, Category="Components|PoseableMesh")
	ENGINE_API void SetBoneTransformByName(FName BoneName, const FTransform& InTransform, EBoneSpaces::Type BoneSpace);

	UFUNCTION(BlueprintCallable, Category="Components|PoseableMesh")
	ENGINE_API void SetBoneLocationByName(FName BoneName, FVector InLocation, EBoneSpaces::Type BoneSpace);

	UFUNCTION(BlueprintCallable, Category="Components|PoseableMesh")
	ENGINE_API void SetBoneRotationByName(FName BoneName, FRotator InRotation, EBoneSpaces::Type BoneSpace);

	UFUNCTION(BlueprintCallable, Category="Components|PoseableMesh")
	ENGINE_API void SetBoneScaleByName(FName BoneName, FVector InScale3D, EBoneSpaces::Type BoneSpace);

	UFUNCTION(BlueprintCallable, Category="Components|PoseableMesh") 
	ENGINE_API FTransform GetBoneTransformByName(FName BoneName, EBoneSpaces::Type BoneSpace);

	UFUNCTION(BlueprintCallable, Category="Components|PoseableMesh")
	ENGINE_API FVector GetBoneLocationByName(FName BoneName, EBoneSpaces::Type BoneSpace);

	UFUNCTION(BlueprintCallable, Category="Components|PoseableMesh")
	ENGINE_API FRotator GetBoneRotationByName(FName BoneName, EBoneSpaces::Type BoneSpace);

	UFUNCTION(BlueprintCallable, Category="Components|PoseableMesh")
	ENGINE_API FVector GetBoneScaleByName(FName BoneName, EBoneSpaces::Type BoneSpace);

	UFUNCTION(BlueprintCallable, Category="Components|PoseableMesh")
	ENGINE_API void ResetBoneTransformByName(FName BoneName);

	UFUNCTION(BlueprintCallable, Category="Components|PoseableMesh")
	ENGINE_API void CopyPoseFromSkeletalComponent(USkeletalMeshComponent* InComponentToCopy);
[...]
```
As mentioned above, in order to modify one bone we need two elements: the name of the bone (or eventually the index), and the desired modification we want to apply (either on location, rotation, scale, or directly the full transform).  
One simple way to get the bones names, is to look directly at the skeletal mesh asset we are using. To open the asset select it in the `Content Browser`
![alt text](images/session1/01-openSkeletalMesh.png "sessionOne-openSkeletalMesh-1")
or directly from the `Details` windows of your posable actor.
![alt text](images/session1/01-openSkeletalMesh-two.png "sessionOne-openSkeletalMesh-1")
Once opened, you should see the following interface. This shows: in the center the view of the skeletal mesh you selected (in this example, I have opened the new Mannequin), and on the right the skeleton hierarchy. This last one shows us the parent-child dependencies of each bone, and the accessing name. 
![alt text](images/session1/02-editorInterface.png "sessionOne-editorInterface")
In this editor interface you can test and manually rotate some of the bones and see how the modification affects the skeleton (**note** don't worry about playing around the change should not affect the skeleton default pose, just be careful of not using the skeleton or mesh editing tools you see on your right). To modify a bone you can simply select it in the hierarchy, see (1) in the following figure, then select the rotating tool on top (2), manually rotate the desired axis, in this case the `y` one, you can also edit values by hand in the `Details` session on the right (4) or either reset to the original pre-modification pose (4). 
![alt text](images/session1/02-editorInterface-editingTotal.png "sessionOne-editorInterface-editing")
As you can notice, if we rotate the shoulder (`upperarm_r` bone), all the bones belonging to the same kinematic chain rotate accordingly. This might seems intuitively, but it requires some background computation, in fact Unreal (like most of the editors) implements **Forward Kinematics (FK)**, to propagate rotation to children bones, and maintain the kinematic connections of the bones. We will use this property also in our code. Another important thing to notice is that most skeletal mesh often have a lot of auxiliary bones, like the one including the `twist` keywords, these kind of bones should not be rotate manually, because their behavior is not always linked to a bone rotation, but instead they serve other functionalities like avoiding mesh artifacts, so we will not consider them in our examples. As well as sockets and other elements that you can find on the hierarchy.

In synthesis: we can apply local and global rotation in our `c++` code to our posable mesh. Remember that, generally, modification of the starting pose should be set in the `BeginPlay` function, while the logic for run-time editing of bones should go into the `Tick` function. This is a simplification not it is not always like this especially for more complex game logics.


Let's see a simple example (waving motion :wave:)
![alt text](images/session1/03-example1-wavingPose.png "sessionOne-example1-wavingPose")
Let's place our character on this waving pose. First things to do is to find what are the appropriate rotation value for the shoulder and the elbow.


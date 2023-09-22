// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PowerPuzzleCharacter.generated.h"


UCLASS(config=Game)
class APowerPuzzleCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Grab, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* ObjectHolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Grab, meta = (AllowPrivateAccess = "true"))
	class UPhysicsHandleComponent* PhysicsHandle;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Left Click Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LeftClickAction;

	/** Right Click Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RightClickAction;

	/** Rotate Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RotateAction;

public:
	APowerPuzzleCharacter();
	
private:
	bool bRightClick = false;

	bool bJustGrabbed = false;
	float RotationDuration = 2.5f;
	float RotationProgress = 0.0f;
	FRotator InitialRotation;
	FRotator TargetRotation;
	float RotationTimer = 0.0f;

	class UPrimitiveComponent* HitObjectComponent;
	class AGrabbableObject* HoveredObjectComponent;

	struct FHitResult LineTrace();

	FRotator ClosestAngleInterval(const FRotator& rotator, float Interval);

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for left click input */
	void LeftClick(const FInputActionValue& Value);

	/** Called for right click input */
	void RightClickTriggered(const FInputActionValue& Value);

	/** Called for right click input */
	void RightClickReleased(const FInputActionValue& Value);

	/** Called for rotate (arrow) input */
	void Rotate(const FInputActionValue& Value);
	
	void Tick(float DeltaTime);
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};


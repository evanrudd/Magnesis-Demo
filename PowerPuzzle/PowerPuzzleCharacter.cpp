// Copyright Epic Games, Inc. All Rights Reserved.

#include "PowerPuzzleCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "PowerPuzzle/Public/GrabbableObject.h"
#include "Kismet/KismetSystemLibrary.h"


//////////////////////////////////////////////////////////////////////////
// APowerPuzzleCharacter

APowerPuzzleCharacter::APowerPuzzleCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	ObjectHolder = CreateDefaultSubobject<USceneComponent>(TEXT("ObjectHolder"));
	ObjectHolder->SetupAttachment(FollowCamera);

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
}

void APowerPuzzleCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input



void APowerPuzzleCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APowerPuzzleCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APowerPuzzleCharacter::Look);

		//Left Click
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Triggered, this, &APowerPuzzleCharacter::LeftClick);

		//Right Click
		EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Triggered, this, &APowerPuzzleCharacter::RightClickTriggered);

		//Right Click
		EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Completed, this, &APowerPuzzleCharacter::RightClickReleased);

		//Rotate
		EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &APowerPuzzleCharacter::Rotate);
	}

}

FHitResult APowerPuzzleCharacter::LineTrace()
{
	FVector LineTraceStart = FollowCamera->GetComponentLocation();
	FVector LineTraceEnd = (LineTraceStart + (FollowCamera->GetForwardVector() * 3000));

	FHitResult HitResult;

	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		LineTraceStart,
		LineTraceEnd,
		ECollisionChannel::ECC_Visibility
	);

	return HitResult;
}

FRotator APowerPuzzleCharacter::ClosestAngleInterval(const FRotator& rotator, float Interval)
{

	FRotator closestRotator;
	closestRotator.Pitch = FMath::RoundToInt(rotator.Pitch / Interval) * Interval;
	closestRotator.Yaw = FMath::RoundToInt(rotator.Yaw / Interval) * Interval;
	closestRotator.Roll = FMath::RoundToInt(rotator.Roll / Interval) * Interval;

	return closestRotator;

}

void APowerPuzzleCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APowerPuzzleCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		if (!bRightClick)
		{
			// add yaw and pitch input to controller
			AddControllerYawInput(LookAxisVector.X);
			AddControllerPitchInput(LookAxisVector.Y);
		}
		else if (HitObjectComponent)
		{
			HitObjectComponent->GetAttachmentRoot()->AddLocalRotation(FRotator(0, LookAxisVector.Y, LookAxisVector.X));
		}
	}
	
}

void APowerPuzzleCharacter::LeftClick(const FInputActionValue& Value)
{
	if (HitObjectComponent != nullptr)
	{
		PhysicsHandle->ReleaseComponent();
		HitObjectComponent = nullptr;
	}
	else
	{
		FHitResult HitResult = LineTrace();

		// -= DEBUG =-
		//FString Name = HitResult.GetActor()->GetName();
		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, (TEXT("Hit Actor: %s"), Name));
		//UKismetSystemLibrary::DrawDebugLine(this, LineTraceStart, LineTraceEnd, FColor::Blue, 5.f, 2.f);

		if (AGrabbableObject* GrabbableObject = Cast<AGrabbableObject>(HitResult.GetActor()))
		{
			if (GrabbableObject->GetCanGrab())
			{
				HitObjectComponent = HitResult.GetComponent();
				InitialRotation = HitResult.GetActor()->GetActorRotation();
				RotationTimer = 0.f;
				RotationProgress = 0.f;
				RotationDuration = 2.5f;

				TargetRotation = ClosestAngleInterval(InitialRotation, 45.f);
				bJustGrabbed = true;

				PhysicsHandle->GrabComponentAtLocationWithRotation(HitObjectComponent, FName("None"), HitResult.GetActor()->GetActorLocation(), InitialRotation);
				GrabbableObject->SetMaterial(EMaterial::EM_Grabbed);
			}
		}
	}
}

void APowerPuzzleCharacter::RightClickTriggered(const FInputActionValue& Value)
{
	bRightClick = true;
}

void APowerPuzzleCharacter::RightClickReleased(const FInputActionValue& Value)
{
	bRightClick = false;
}

void APowerPuzzleCharacter::Rotate(const FInputActionValue& Value)
{
	if (HitObjectComponent != nullptr)
	{
		FVector2D RotateVector = Value.Get<FVector2D>();

		FRotator AddedRotation = FRotator(RotateVector.X * 45, RotateVector.Y * 45, 0);

		//HitObjectComponent->GetAttachmentRoot()->AddWorldRotation(AddedRotation);
		InitialRotation = HitObjectComponent->GetAttachParent()->GetRelativeRotation();

		RotationTimer = 0.f;
		RotationDuration = 0.5f;
		RotationProgress = 0.f;
		HitObjectComponent->GetAttachmentRoot()->AddLocalRotation(AddedRotation);
		//TargetRotation = InitialRotation + AddedRotation; // AddedRotation;
		//bJustGrabbed = true;
	}
}

void APowerPuzzleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (HitObjectComponent != nullptr)
	{
		PhysicsHandle->SetTargetLocation(ObjectHolder->GetComponentLocation());

		if (bJustGrabbed)
		{
			RotationTimer += DeltaTime;
			RotationProgress = FMath::Clamp(RotationTimer / RotationDuration, 0.0f, 1.0f);
			FRotator NewRotation = FMath::Lerp(InitialRotation, TargetRotation, RotationProgress);
			HitObjectComponent->GetAttachmentRoot()->SetRelativeRotation(NewRotation);

			if (RotationProgress >= 1.0f)
			{
				bJustGrabbed = false;
			}
		}
	}
	else
	{
		FHitResult HitResult = LineTrace();
		if (AGrabbableObject* GrabbableObject = Cast<AGrabbableObject>(HitResult.GetActor()))
		{
			if (GrabbableObject->GetCanGrab())
			{
				HoveredObjectComponent = GrabbableObject;
				GrabbableObject->SetMaterial(EMaterial::EM_Hover);
			}
		}
		else if (HoveredObjectComponent)
		{
			HoveredObjectComponent->SetMaterial(EMaterial::EM_Default);
			HoveredObjectComponent = nullptr;
		}
	}
}

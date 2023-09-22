// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrabbableObject.generated.h"

enum class EMaterial : uint8
{
	EM_Default,
	EM_Hover,
	EM_Grabbed
};


UCLASS()
class POWERPUZZLE_API AGrabbableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	AGrabbableObject();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = "Grab")
	FORCEINLINE void SetIsGrabbed(bool Value) { bIsGrabbed = Value; }

	UFUNCTION(BlueprintPure, Category = "Grab")
	FORCEINLINE bool GetIsGrabbed() const { return bIsGrabbed; }

	UFUNCTION(Category = "Grab")
	FORCEINLINE void SetCanGrab(bool Value) { bCanGrab = Value; }

	UFUNCTION(BlueprintPure, Category = "Grab")
	FORCEINLINE bool GetCanGrab() const { return bCanGrab; }

	void SetMaterial(EMaterial Material);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grab")
	bool bIsGrabbed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grab")
	bool bCanGrab;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, Category = "Grab")
	UMaterial* Default;

	UPROPERTY(EditAnywhere, Category = "Grab")
	UMaterial* DefaultMaterial;

	UPROPERTY(EditAnywhere, Category = "Grab")
	UMaterial* HoverMaterial;

	UPROPERTY(EditAnywhere, Category = "Grab")
	UMaterial* GrabMaterial;
};

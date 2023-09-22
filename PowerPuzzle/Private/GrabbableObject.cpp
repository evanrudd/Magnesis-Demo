// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabbableObject.h"

// Sets default values
AGrabbableObject::AGrabbableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;
}

void AGrabbableObject::SetMaterial(EMaterial Material)
{
	switch (Material)
	{
	case EMaterial::EM_Default:
		ItemMesh->SetMaterial(0, DefaultMaterial);
		break;
	case EMaterial::EM_Hover:
		ItemMesh->SetMaterial(0, HoverMaterial);
		break;
	case EMaterial::EM_Grabbed:
		ItemMesh->SetMaterial(0, GrabMaterial);
		break;
	default:
		break;
	}
}

// Called when the game starts or when spawned
void AGrabbableObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrabbableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


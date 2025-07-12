// Fill out your copyright notice in the Description page of Project Settings.


#include "PriceTag.h"
#include "Components/TextRenderComponent.h"

// Sets default values
APriceTag::APriceTag()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = Root;

	PriceText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Price Text"));
	PriceText->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void APriceTag::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APriceTag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


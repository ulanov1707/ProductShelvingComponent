// Fill out your copyright notice in the Description page of Project Settings.


#include "ProductCrate.h"
#include "Components/TextRenderComponent.h"
#include "Components/DecalComponent.h"
#include "UObject/UnrealType.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AProductCrate::AProductCrate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SetReplicates(true);
	SetReplicateMovement(true);

	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BoxMesh"));
	RootComponent = BoxMesh;

	Count = CreateDefaultSubobject<UTextRenderComponent>(FName("Count"));
	Name = CreateDefaultSubobject<UTextRenderComponent>(FName("Name"));
	ProductType = CreateDefaultSubobject<UTextRenderComponent>(FName("ProductType"));
	ProductIcon = CreateDefaultSubobject<UDecalComponent>(FName("ProductIcon"));

	Count->SetupAttachment(RootComponent);
	Name->SetupAttachment(RootComponent);
	ProductType->SetupAttachment(RootComponent);
	ProductIcon->SetupAttachment(RootComponent);
	
	

}

// Called when the game starts or when spawned
void AProductCrate::BeginPlay()
{
	Super::BeginPlay();

	RefreshData();
	
}

void AProductCrate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProductCrate, ProductCount);
	DOREPLIFETIME(AProductCrate, Product);
}
//set ups all visual data on the BOX
void AProductCrate::RefreshData()
{
	Name->SetText(Product.Name);
	ProductType->SetText(UEnum::GetDisplayValueAsText(Product.ProductType));
	Count->SetText(FText::Format(FText::FromString("x{0}"), ProductCount));

	if (bUseProductIcon) 
	{
		if (Product.ProductIcon) 
		{
			ProductIcon->SetDecalMaterial(Product.ProductIcon);
		}
	}
	else 
	{
		ProductIcon->DestroyComponent();
	}

}

// Called every frame
void AProductCrate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProductCrate::AddProduct()
{
	ProductCount++;
	RefreshData();
}

void AProductCrate::RemoveProduct()
{
	ProductCount--;
	if (ProductCount == 0) 
	{
		Product = FProductStruct();
		RefreshData();
	}
	else 
	{
		RefreshData();
	}
}

void AProductCrate::OnRep_ProductCount()
{
	RefreshData();
}


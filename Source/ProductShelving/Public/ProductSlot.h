// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EProductTypes.h"
#include "Interfaces/ShelvingInterface.h"
#include "ProductStruct.h"
#include "ProductSlot.generated.h"

USTRUCT()
struct FShelfSetupOutput
{
	GENERATED_BODY()

public:
	
	FShelfSetupOutput()
	{
		bSetupComplete = false;
		Notification = EProductNotification::NoSpaceForProduct;
	}

	FShelfSetupOutput(bool bInComplete, EProductNotification InNotification)
	{
		bSetupComplete = bInComplete;
		Notification = InNotification;
	}

	UPROPERTY()
	bool bSetupComplete = false;

	UPROPERTY()
	EProductNotification Notification;
};

UCLASS()
class PRODUCTSHELVING_API AProductSlot : public AActor,public IShelvingInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProductSlot();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Notify(EProductNotification Notification,const AActor * Instigator);
	void ResetShelf();
	FShelfSetupOutput SetupShelf(AProductCrate* Crate, bool bReplication,const AActor* Instigator);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	bool RemoveFromShelf(class AProductCrate* CarriedCrate, const AActor* Instig)override;
	

	bool AddToShelf(class AProductCrate* CarriedCrate, const AActor* Instig)override;

private:
	//NOTE : This functions takes refs!!!
	bool CalculateAndValidateX(float & XValue);
	bool CalculateAndValidateY(float& YValue);
	bool CalculateAndValidateZ(float& ZValue);
	FShelfSetupOutput HandleShelfSetup(AProductCrate* Crate, bool bReplication,const AActor* Instigator);
	void AddPricetag(AActor* PriceTag);
	void CenterProductX(uint8 Index);
	void CenterProductY(uint8 Index);

	float GetRandomRotationRange();

	void HandleResetting();
	void InitSlot();

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootSceneComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UInstancedStaticMeshComponent* InstancedStaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ArrowComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Plane;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere)
	UBillboardComponent* Billboard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* PriceTag;


	UPROPERTY(EditAnywhere, Category = "VFX")
	UParticleSystem* SpawnVFX;
	UPROPERTY(EditAnywhere, Category = "VFX")
	FVector VFXScale = FVector(1.f);

	// variables for calculation

	UPROPERTY()
	FVector2D ActiveProductSpacing;

	//just kind of a utility varibale used for calcualtion how many max products per row  shelf can  have 
	UPROPERTY()
	uint8 Products;

	//just kind of a utility varibale used for calcualtion how many max rows  shelf can  have 
	UPROPERTY()
	uint8 Rows;

	UPROPERTY()
	uint8 Stack;

	UPROPERTY()
	uint8 MaxRows;

	UPROPERTY()
	uint8 ProductsPerRow;

	UPROPERTY()
	bool bEmpty = true;

	UPROPERTY()
	bool CenterX = true;

	UPROPERTY()
	bool CenterY = true;

	UPROPERTY()
	float CurrentX;

	UPROPERTY()
	float CurrentY;
	
	UPROPERTY()
	float TargerCenterX;

	UPROPERTY()
	float TargerCenterY;

// variables for visuals 

	UPROPERTY(EditAnywhere,Category = "Visual")
	bool bUseVisualEffects = false;
	UPROPERTY(EditAnywhere, Category = "Visual")
	float SpringSeed = 15.f;

	UPROPERTY(EditAnywhere, Category = "Visual/Spacing")
	bool bUseSpacingFromProduct = false;
	UPROPERTY(EditAnywhere, Category = "Visual/Spacing")
	FVector2D  ShelfProductSpacing = FVector2D(1.f,1.f);

// Price Tag 
	UPROPERTY(EditAnywhere, Category = "PriceTag")
	FTransform PricetagOffset = FTransform(FVector(0.f, 5.f, 0.f));

	UPROPERTY(VisibleAnywhere,Category = "PriceTag")
	TArray<class APriceTag*> PriceTags;

	UPROPERTY(EditAnywhere, Category = "PriceTag")
	bool bUsePriceTag = false;

//Shelf 
	UPROPERTY(EditAnywhere, Category = "Shelf")
	bool bResetEmptyShelf = true;

	UPROPERTY(EditAnywhere)
	EProductType ProductType = EProductType::Default;

//Replication
	
	
	UPROPERTY(VisibleAnywhere,Replicated)
	FProductStruct Product;


	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_ProductCount)
	uint8 ProductCount;

	// how many items of product this shelf has
	UFUNCTION()
	void OnRep_ProductCount();












	

};

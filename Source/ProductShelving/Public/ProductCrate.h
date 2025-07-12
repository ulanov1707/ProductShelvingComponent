// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProductStruct.h"
#include "ProductCrate.generated.h"

UCLASS()
class PRODUCTSHELVING_API AProductCrate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProductCrate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void RefreshData();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AddProduct();
	void RemoveProduct();

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "Product")
	UStaticMeshComponent* BoxMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Product")
	class UTextRenderComponent* Count;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Product")
	class UTextRenderComponent* Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Product")
	class UTextRenderComponent* ProductType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Product")
	UDecalComponent* ProductIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product",Replicated)
	FProductStruct Product;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product")
	bool bUseProductIcon = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product", ReplicatedUsing = OnRep_ProductCount)
	uint8 ProductCount = 0;

	UFUNCTION()
	void OnRep_ProductCount();

};

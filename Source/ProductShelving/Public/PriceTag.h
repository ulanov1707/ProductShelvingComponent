// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PriceTag.generated.h"


 

UCLASS()
class PRODUCTSHELVING_API APriceTag : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APriceTag();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE class UTextRenderComponent* GetPriceTag() 
	{
		return PriceText;
	}
private:


	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	class UTextRenderComponent* PriceText;

};

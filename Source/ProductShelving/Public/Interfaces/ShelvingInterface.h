// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ShelvingInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI,Blueprintable)
class UShelvingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PRODUCTSHELVING_API IShelvingInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual bool AddToShelf(class AProductCrate * CarriedCrate,const AActor * Instigator) = 0;

	
	virtual bool RemoveFromShelf(class AProductCrate* CarriedCrate, const AActor* Instigator) = 0;
};

#pragma once

#include "CoreMinimal.h"
#include "EProductTypes.generated.h"

UENUM(BlueprintType)
enum class EProductType : uint8
{
    Default        UMETA(DisplayName = "Default"),
    Frozen        UMETA(DisplayName = "Frozen"),
    Cold        UMETA(DisplayName = "Cold"),
    Warm         UMETA(DisplayName = "Warm"),
    Fruit     UMETA(DisplayName = "Fruit")
};

UENUM(BlueprintType)
enum class EProductNotification : uint8
{
    NoSpaceForProduct        UMETA(DisplayName = "No space for product"),
    ProductNotMatching       UMETA(DisplayName = "Product not matching"),
    EmptyProductCrate        UMETA(DisplayName = "Empty product crate"),
    WrongProductType         UMETA(DisplayName = "Wrong product type"),
    EmptyShelf               UMETA(DisplayName = "Empty shelf"),
    ProductNotStackable      UMETA(DisplayName = "Product not stackable"),
    ProductCrateIsFull       UMETA(DisplayName = "Product Crate is full")

};


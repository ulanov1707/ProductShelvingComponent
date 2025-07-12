#pragma once

#include "CoreMinimal.h"
#include "EProductTypes.h"
#include "ProductStruct.generated.h"

USTRUCT(BlueprintType)
struct FProductStruct
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product")
    UStaticMesh* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product")
    float Price = 0.f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product")
    EProductType ProductType = EProductType::Default;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product")
    FText Name = FText::FromString(TEXT("Product Name"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product")
    bool bStackable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product")
    FVector2D RotationRange = FVector2D(0.f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product")
    float ZMultiply = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product")
    FVector2D ProductSpacing = FVector2D(1.f, 1.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product")
    uint8 MaxCrateCapacity = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product")
    UMaterialInstance* ProductIcon;
    
    FProductStruct() {}
};
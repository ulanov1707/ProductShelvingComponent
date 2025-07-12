// Fill out your copyright notice in the Description page of Project Settings.


#include "ProductSlot.h"
#include "ShelvingComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "Net/UnrealNetwork.h" 
#include "PriceTag.h"
#include "Components/TextRenderComponent.h"
#include "ProductCrate.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values
AProductSlot::AProductSlot()
{
 	//initially tick disabled 
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);
	
	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootSceneComp;

	InstancedStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMesh"));
	InstancedStaticMesh->SetupAttachment(RootComponent);

	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Component"));
	ArrowComp->SetupAttachment(InstancedStaticMesh);

	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	Plane->SetupAttachment(RootComponent);

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(Plane);

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);

	PriceTag = CreateDefaultSubobject<UChildActorComponent>(TEXT("Price Tag"));
	PriceTag->SetupAttachment(RootComponent);


	InitSlot();
	
}

void AProductSlot::InitSlot()
{


	Box->SetRelativeLocation(FVector(0.f, 0.f, Box->GetScaledBoxExtent().Z));
	if(bUsePriceTag)
	{
		FVector Min, Max;
		Plane->GetLocalBounds(Min, Max);
		FVector PriceTagLocation = FVector(PricetagOffset.GetLocation().X, PricetagOffset.GetLocation().Y + Max.Y, PricetagOffset.GetLocation().Z);
		PriceTag->SetRelativeLocation(PriceTagLocation);
		FRotator PriceTagRotation = FRotator(PricetagOffset.GetRotation().X, PricetagOffset.GetRotation().Y, PricetagOffset.GetRotation().Z + 90.f);
		PriceTag->SetRelativeRotation(PriceTagRotation);
		AddPricetag(PriceTag->GetChildActor());

	}
	else
	{
		PriceTag->DestroyComponent();
	}

	
}
void AProductSlot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProductSlot, Product);
	DOREPLIFETIME(AProductSlot, ProductCount);
}


// Called when the game starts or when spawned
void AProductSlot::BeginPlay()
{
	Super::BeginPlay();
	
	// so you can adjust actor scale and it won't affect Instacnieted meshes's Scale and calculations will be fine 
	InstancedStaticMesh->SetRelativeScale3D(FVector(1.f / GetActorScale3D().X, 1.f / GetActorScale3D().Y, 1.f / GetActorScale3D().Z));

	
}


// Called every frame
void AProductSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Removes product from Slot and Adds Product to Carried crate 
bool AProductSlot::RemoveFromShelf(AProductCrate* CarriedCrate, const AActor* Instig)
{
	if (HasAuthority()) 
	{
		if (CarriedCrate)
		{
			//is shelf empty
			if (bEmpty)
			{
				Notify(EProductNotification::EmptyShelf, Instig);
				return false;
			}
			else
			{
				//does crate has space
				if (CarriedCrate->ProductCount < CarriedCrate->Product.MaxCrateCapacity)
				{
					//if product in carried crate and product in current shelf is not equal
					if (!CarriedCrate->Product.Name.EqualTo(Product.Name))
					{
						// can change produt type if ther is no product on shelf
						if (CarriedCrate->ProductCount == 0)
							CarriedCrate->Product = Product;
						else
							return false;
					}
				}
				else
				{
					Notify(EProductNotification::ProductCrateIsFull, Instig);
					return false;
				}

			}
		}
		else
			return false;
	}

	//For Client or Continiue of Server code

	if (Products <= 0) 
	{
		if (Rows > 0) 
		{
			Rows--;
			Products = ProductsPerRow;
		}
		else 
		{
			Rows = MaxRows - 1;
			Products = ProductsPerRow;
			Stack--;
		}
	}

	if (InstancedStaticMesh && InstancedStaticMesh->IsValidInstance(InstancedStaticMesh->GetInstanceCount() - 1))
	{
		// if could remove last instance
		if (InstancedStaticMesh->RemoveInstance(InstancedStaticMesh->GetInstanceCount() - 1))
		{
			if (HasAuthority())
			{
				ProductCount--;
				ForceNetUpdate();
			}

			Products--;
			if (ProductCount == 0)
			{
				if (bResetEmptyShelf)
					ResetShelf();
			}

			//removed from Shelf and added to carried crate
			if (HasAuthority() && CarriedCrate)
			{
				CarriedCrate->AddProduct();
			}

			return true;
		}
		else
			return false;
	}
	else
		return false;
	
}

bool AProductSlot::AddToShelf(AProductCrate* CarriedCrate, const AActor* Instig)
{
	if (HasAuthority()) //Server
	{	
		if (CarriedCrate)
		{
			//Setup product shelf if its empty & if crate has products (SERVER)
			if (bEmpty)
			{
				//try to setup shelf 
				FShelfSetupOutput ShelfOutput = SetupShelf(CarriedCrate, false, Instig);

				//couldnt setup shelf , and notifys why
				if (!ShelfOutput.bSetupComplete)
				{
					Notify(ShelfOutput.Notification, Instig);
					return false;
				}

				//if shelf Setup successfully check if crate doent have items
				if (CarriedCrate->ProductCount <= 0)
				{
					Notify(EProductNotification::EmptyProductCrate, Instig);
					return false;
				}

				//check shelf requirements 
				if (CarriedCrate->Product.Name.EqualTo(Product.Name)) 
				{
					//if wrong product type 
					if (Product.ProductType != ProductType) 
					{
						Notify(EProductNotification::WrongProductType, Instig);
						return false;
					}
						
					
				}
				else // not matching product name
				{
					Notify(EProductNotification::ProductNotMatching,Instig);
				}


			}

		}
		else
			return false;
	
	}
	else //Client 
	{
		if (bEmpty) 
		{
			FShelfSetupOutput ShelfOutput =	SetupShelf(nullptr, true, nullptr);
			
			//if couldn't setup
			if (!ShelfOutput.bSetupComplete) 
			{
				Notify(ShelfOutput.Notification, Instig);
				return false;
			}
		}
	}

	//Caclulate X placement and check if it has space , if don;t have space try next row and first element
	float X,Y,Z;
	if (!CalculateAndValidateX(X)) 
	{
		Rows++;
		Products = 0;
	}
	//Caclulate Y placement and check if it has space , if don't have space try to stack it(put on top of products)
	if (!CalculateAndValidateY(Y)) 
	{
		if (Product.bStackable) 
		{
			Rows = 0;
			Stack++;
		}
		else 
		{
			Notify(EProductNotification::ProductNotStackable,Instig);
			return false;
		}
	}
	if (!CalculateAndValidateZ(Z)) 
	{
		Notify(EProductNotification::NoSpaceForProduct, Instig);
		return false;
	}

	//If reached here that means it has enough space on all dimensions
 	
		//Add static product instance(Instanced static mesh)
	if (CalculateAndValidateX(X) && CalculateAndValidateY(Y) && CalculateAndValidateZ(Z))
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			222.f,
			FColor::Yellow,
			FString::Printf(TEXT("Index: %d  Value X:%f "), Products,X)
		);

		

		FTransform InstanceStaticMeshTransform = FTransform(FRotator(0.f, 0.f, GetRandomRotationRange()), FVector(X, Y, Z), FVector(1.f, 1.f, 1.f));
		uint8 InstancedMeshIndex = InstancedStaticMesh->AddInstance(InstanceStaticMeshTransform);

		
		InstancedStaticMesh->GetInstanceTransform(InstancedMeshIndex, InstanceStaticMeshTransform, true);

		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			SpawnVFX,
			InstanceStaticMeshTransform.GetLocation(),
			InstanceStaticMeshTransform.GetRotation().Rotator(),
			VFXScale,  // scale
			true           // auto destroy
			);

		//Increment product count
		if (HasAuthority())
		{
			ProductCount++;
			ForceNetUpdate();
		}

		Products++;

		//Center products
		if (ProductsPerRow > 0 && CenterX)
			CenterProductX(InstancedMeshIndex);
		if (MaxRows > 0 && CenterY)
			CenterProductY(InstancedMeshIndex);

		//Set product shelf to not empty if its empty
		if (bEmpty)
			bEmpty = false;

		//Remove product from crate if its valid
		if (HasAuthority())
		{
			if (CarriedCrate)
				CarriedCrate->RemoveProduct();
		}

		//SUCCESSFULLY ADDDED PRODUCT
		return true;

	}
	return false;
}

// compares the plane lenght on X with the lenght of spawned products lenght on X on this row , if PlaneLenght is higher returns HasSpace-true
//XValue is a local position of the next spawnable point 
bool AProductSlot::CalculateAndValidateX(float& XValue)
{
	
	FVector Min, Max;
	//half lenght of mesh (FIRST MESH ONLY , not whole Instanced Meshes) on X 
	InstancedStaticMesh->GetLocalBounds(Min, Max);

	float MinX = Min.X;
	float MaxX = Max.X;


	float NLocalBounds;

	//we are choosing the longest one to be safe 
	if (FMath::Abs(MinX) >= FMath::Abs(MaxX)) 
	{
		NLocalBounds = FMath::Abs(MinX);
	}
	else 
	{
		NLocalBounds = FMath::Abs(MaxX);
	}

	// half lenght of plane mesh (if you change the scale of Mesh , result will be the same as before , so don't forget to multypli it by actror scale)
	Plane->GetLocalBounds(Min, Max);

	//we are comparing here 2 lines 
	
	float PlaneLenght = FMath::Abs(Min.X) * GetActorScale3D().X * 2.f;
	float TotalLenghtOfProductsOnThisRow = (NLocalBounds + ActiveProductSpacing.X) * 2.f * (Products + 1);

	bool bHasSpace = TotalLenghtOfProductsOnThisRow <= PlaneLenght;
		
	//Position of the next product to be added on X
	XValue = (NLocalBounds + ActiveProductSpacing.X) * 2.f * (Products + 1) - NLocalBounds - ActiveProductSpacing.X;
	
	return bHasSpace;
}

bool AProductSlot::CalculateAndValidateY(float& YValue)
{
	FVector Min, Max;
	InstancedStaticMesh->GetLocalBounds(Min, Max);

	float MinY = Min.Y;
	float MaxY = Max.Y;

	float NLocalBounds;

	if (FMath::Abs(MinY) >= FMath::Abs(MaxY))
	{
		NLocalBounds = FMath::Abs(MinY);
	}
	else
	{
		NLocalBounds = FMath::Abs(MaxY);
	}

	//Y and space calculations
	Plane->GetLocalBounds(Min, Max);

	float PlaneLenght = FMath::Abs(Min.Y) * GetActorScale3D().Y * 2.f;
	float TotalLenghtOfProductsOnThisRow = (NLocalBounds + ActiveProductSpacing.Y) * 2.f * (Rows + 1);

	bool bHasSpace = TotalLenghtOfProductsOnThisRow <= PlaneLenght;

	//Position of the next product to be added on X
	YValue = (NLocalBounds + ActiveProductSpacing.Y) * 2.f * (Rows + 1) - NLocalBounds - ActiveProductSpacing.Y;

	return bHasSpace;
}

bool AProductSlot::CalculateAndValidateZ(float& ZValue)
{
	FVector Min, Max;
	InstancedStaticMesh->GetLocalBounds(Min, Max);

	float MinZ = Min.Z;
	float MaxZ = Max.Z;

	float NLocalBounds;

	if (FMath::Abs(MinZ) >= FMath::Abs(MaxZ))
	{
		NLocalBounds = FMath::Abs(MinZ);
	}
	else
	{
		NLocalBounds = FMath::Abs(MaxZ);
	}
	//Z and space calculations
	
	ZValue = (Product.ZMultiply * ((FMath::Abs(MinZ) >= FMath::Abs(MaxZ)) ? 2 : 1) * FMath::Abs(MaxZ) * Stack) + FMath::Abs(MinZ);
	bool bHasSpace = (ZValue + NLocalBounds) <= (Box->GetScaledBoxExtent().Z * 2.f);// GetScaledBoxExtend MIGHT cause issues in future if so better to use GetUnscaledBoxExtned and multiply it by GetActorScale3D
	
	return bHasSpace;
}




//Run a loop while product count is not equal to number of instances in static mesh, then add / remove depending on difference
void AProductSlot::OnRep_ProductCount()
{
	while (InstancedStaticMesh->GetInstanceCount() != ProductCount) 
	{
		if (InstancedStaticMesh->GetInstanceCount() < ProductCount)
		{
			AddToShelf(nullptr, nullptr);
		}
		else 
		{
			RemoveFromShelf(nullptr, nullptr);
		}
	}
}

void AProductSlot::Notify(EProductNotification Notification,const AActor* Instig)
{
	if (HasAuthority() && Instig)
	{
		UShelvingComponent * ShelfComp = Instig->GetComponentByClass<UShelvingComponent>();
		if (ShelfComp) 
		{
			ShelfComp->Client_Notify(Notification);
		}
		else 
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Missing Shelving Component");
		}
	}
	
}

void AProductSlot::ResetShelf()
{
	if (HasAuthority())
	{
		ProductCount = 0;
		Product = FProductStruct();
		ForceNetUpdate();
		HandleResetting();
	}
	else
	{
		HandleResetting();
	}


}

void AProductSlot::HandleResetting()
{
	if (!bEmpty)
	{
		SetActorTickEnabled(false);
		FVector Min, Max;
		Plane->GetLocalBounds(Min, Max);
		FVector ResetLocation = Min - Plane->GetRelativeLocation();
		InstancedStaticMesh->SetRelativeLocation(FVector(ResetLocation.X, ResetLocation.Y, 0.f));
		InstancedStaticMesh->SetStaticMesh(nullptr);
		bEmpty = true;
		ProductsPerRow = 0;
		Rows = 0;
		Products = 0;
		Stack = 0;
		MaxRows = 0;
		CenterX = 0;
		CenterY = 0;
		CurrentX = 0.f;
		CurrentY = 0.f;
		TargerCenterX = 0.f;
		TargerCenterY = 0.f;

		if (InstancedStaticMesh->GetInstanceCount() > 0)
		{
			InstancedStaticMesh->ClearInstances();

			for (APriceTag* Price : PriceTags)
			{
				Price->GetPriceTag()->SetText(FText::FromString(TEXT("$0.00")));
			}
		}
		else
		{
			for (APriceTag* Price : PriceTags)
			{
				Price->GetPriceTag()->SetText(FText::FromString(TEXT("$0.00")));
			}
		}


	}
}



FShelfSetupOutput AProductSlot::SetupShelf(AProductCrate* Crate, bool bReplication,const AActor* Instig)
{
	
	if (HasAuthority()) 
	{
		if (Crate && (Crate->ProductCount > 0)) 
		{
			EProductType CrateProductType = Crate->Product.ProductType;
			if (ProductType == CrateProductType) 
			{
				Product = Crate->Product;
				ForceNetUpdate();
				
			}
			else 
			{	
				return FShelfSetupOutput(false, EProductNotification::WrongProductType);
			}
		}
		else 
		{
			return FShelfSetupOutput(false, EProductNotification::EmptyProductCrate);
		}

		return HandleShelfSetup(Crate,bReplication,Instig);
	}
	else 
	{
		 return HandleShelfSetup(Crate, bReplication, Instig);
	}

	
}
//sets up shelf 
//calculate MaxRows/PRoductsPerRow to be able to add on this shelf
//sets up product type,mesh of product , spacing etc
FShelfSetupOutput AProductSlot::HandleShelfSetup(AProductCrate* Crate, bool bReplication,const AActor* Instig)
{
	//Setting Mesh of product
	UStaticMesh* MeshToSet = (bReplication ? Crate->Product : Product).Mesh;
	InstancedStaticMesh->SetStaticMesh(MeshToSet);

	//Setting correct spacing between meshes
	if (bUseSpacingFromProduct) 
		ActiveProductSpacing = Product.ProductSpacing;
	else 
		ActiveProductSpacing = ShelfProductSpacing;

	float X, Y, Z;

	// does has space on all dimensions ?
	if (CalculateAndValidateX(X) && CalculateAndValidateY(Y) && CalculateAndValidateZ(Z)) 
	{
		// setup PriceTag Text
		for (APriceTag* Price : PriceTags)
		{
			float PriceValue = (bReplication ? Crate->Product : Product).Price;
			FString PriceStr = FString::Printf(TEXT("$%.2f"), PriceValue);
			Price->GetPriceTag()->SetText(FText::FromString(PriceStr));
		}

		
		bool bBreak = false;
		//Calculate how many Maximum Products per row this shelf can have and how many maximum Rows this shelf can have 
		while ((ProductsPerRow == 0 || MaxRows == 0 ) && !bBreak)
		{
		//Products Per row
			if(ProductsPerRow == 0)
			{
				//does has space on X 
				if (CalculateAndValidateX(X)) 
				{
					Products++;

				}
				else 
				{
					if (Products == 0) 
					{
						bBreak = true;
					}
					else 
					{
						ProductsPerRow = Products;
						Products = 0;
					}
				}
			}

		// Rows 
			if (MaxRows == 0)
			{
				//does has space on Y
				if (CalculateAndValidateY(Y))
				{
					Rows++;
				}
				else
				{
					if (Rows == 0)
					{
						bBreak = true;
					}
					else
					{
						MaxRows = Rows;
						Rows = 0;
					}
				}
			}	
		}

		return FShelfSetupOutput(true, EProductNotification::WrongProductType);

	}
	else // don't have space 
	{
		ResetShelf();
		return FShelfSetupOutput(false, EProductNotification::NoSpaceForProduct);
	}

	
}

void AProductSlot::AddPricetag(AActor* PriceTagg)
{
	APriceTag* PriceTagRef = Cast<APriceTag>(PriceTagg);

	if(PriceTagRef)
		PriceTags.Add(PriceTagRef);
}

void AProductSlot::CenterProductX(uint8 Index)
{
	CenterX = false;

	float  NLocalBounds;
	FVector Max, Min;
	InstancedStaticMesh->GetLocalBounds(Min, Max);

	//Check which bound value to use from pivot point
	if (FMath::Abs(Min.X) >= FMath::Abs(Max.X))
	{
		NLocalBounds = Min.X;
	}
	else
	{
		NLocalBounds = Max.X;
	}

	FTransform InstanceTransform;
	InstancedStaticMesh->GetInstanceTransform((int32)Index, InstanceTransform);

	//Get edge of last instance to calculate distance to surface edge
	float EdgeOfLastInstance = ((InstanceTransform.GetLocation().X + (NLocalBounds + ActiveProductSpacing.X)) * ProductsPerRow)/ GetActorScale3D().X;

	//Get surface bounds
	Plane->GetLocalBounds(Min, Max);

	//Calculate target center for X (Relative)
	float RelativeTargetCenterX = EdgeOfLastInstance - (Max.X * 2.f);
	TargerCenterX = FMath::Abs(RelativeTargetCenterX) / 2.f;

	
		InstancedStaticMesh->AddLocalOffset(FVector(TargerCenterX, 0.f, 0.f));
	
}

void AProductSlot::CenterProductY(uint8 Index)
{
	CenterY = false;
	float  NLocalBounds;
	FVector Max, Min;
	InstancedStaticMesh->GetLocalBounds(Min, Max);

	//Check which bound value to use from pivot point
	if (FMath::Abs(Min.Y) >= FMath::Abs(Max.Y))
	{
		NLocalBounds = Min.Y;
	}
	else
	{
		NLocalBounds = Max.Y;
	}

	FTransform InstanceTransform;
	InstancedStaticMesh->GetInstanceTransform((int32)Index, InstanceTransform);

	//Get edge of last instance to calculate distance to surface edge
	float EdgeOfLastInstance = ((InstanceTransform.GetLocation().Y + (NLocalBounds + ActiveProductSpacing.Y)) * MaxRows) / GetActorScale3D().Y;

	//Get surface bounds
	Plane->GetLocalBounds(Min, Max);

	//Calculate target center for X (Relative)
	float RelativeTargetCenterY = EdgeOfLastInstance - (Max.Y * 2.f);
	TargerCenterX = FMath::Abs(RelativeTargetCenterY) / 2.f;

	
	InstancedStaticMesh->AddLocalOffset(FVector(0.f, TargerCenterY, 0.f));
	

}

float AProductSlot::GetRandomRotationRange()
{
	return FMath::FRandRange(Product.RotationRange.X, Product.RotationRange.Y);
}


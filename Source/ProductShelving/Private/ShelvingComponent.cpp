// Fill out your copyright notice in the Description page of Project Settings.


#include "ShelvingComponent.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/ShelvingInterface.h"
#include "Net/UnrealNetwork.h"
#include "ProductCrate.h"
#include "Camera/CameraComponent.h"


// Sets default values for this component's properties
UShelvingComponent::UShelvingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	// ...
}

void UShelvingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UShelvingComponent, CarriedBox);
}


void UShelvingComponent::Client_Notify_Implementation(EProductNotification Notification)
{
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, UEnum::GetDisplayValueAsText(Notification).ToString());
}

// Called when the game starts
void UShelvingComponent::BeginPlay()
{
	Super::BeginPlay();

    OwnerPawn = Cast<APawn>(GetOwner());
    // Only bind input on locally controlled actors
    if (OwnerPawn)
    {
        // Only run on the owning client
       // if (!OwnerPawn || OwnerPawn->GetLocalRole() != ROLE_AutonomousProxy) return;

        if (OwnerPawn->IsLocallyControlled())
        {
            APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
            if (!PC)
            {
                UE_LOG(LogTemp, Warning, TEXT("Owner Pawn has no PlayerController!"));
                return;
            }

            // Enable input on the owner actor for this controller
            OwnerPawn->EnableInput(PC);

            if (OwnerPawn->InputComponent)
            {
                SetupInput(OwnerPawn->InputComponent);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("InputComponent not found on owner pawn!"));
            }
        }
    }
	
}



// Called every frame
void UShelvingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}





void UShelvingComponent::SetupInput(UInputComponent* PlayerInputComponent)
{
    if (!PlayerInputComponent) return;

    //PlayerInputComponent->BindAction("LazyPoductAdd", IE_Pressed, this, &UShelvingComponent::OnAddLazy);
    //PlayerInputComponent->BindAction("LazyPoductRemove", IE_Pressed, this, &UShelvingComponent::OnRemoveLazy);
    PlayerInputComponent->BindAction("AddSingleProduct", IE_Pressed, this, &UShelvingComponent::OnAddSingle);
    PlayerInputComponent->BindAction("RemoveSingleProduct", IE_Pressed, this, &UShelvingComponent::OnRemoveSigle);
    PlayerInputComponent->BindAction("GrabBox", IE_Pressed, this, &UShelvingComponent::OnGrabBox);
    PlayerInputComponent->BindAction("DropBox", IE_Pressed, this, &UShelvingComponent::OnDropBox);
}

//adds 5 products to slot 
void UShelvingComponent::OnAddLazy()
{
  
    for (int i = 0; i < 5; i++)
    {
        OnAddSingle();
    }
}

void UShelvingComponent::OnRemoveLazy()
{

    for (int i = 0; i < 5; i++)
        OnRemoveLazy();

}


// add single product to slot
void UShelvingComponent::OnAddSingle()
{
  
    AActor * HitActor = TraceForCamera();
    if (IsValid(HitActor) && HitActor->Implements<UShelvingInterface>()) 
    {
        
        IShelvingInterface* InterfaceRef = Cast<IShelvingInterface>(HitActor);
        //is Product slot 
        if (InterfaceRef)
        {
            if (GetOwner()->HasAuthority())
            {
                // here is should be Server/Client RPCs 
                InterfaceRef->AddToShelf(CarriedBox, GetOwner());
            }
            else 
            {
                Server_AddToShelf(HitActor, CarriedBox);
            }
        }
    }
}

void UShelvingComponent::OnRemoveSigle()
{
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, "Remove Single Called");
    AActor* HitActor = TraceForCamera();
    if (IsValid(HitActor) && HitActor->Implements<UShelvingInterface>())
    {

        IShelvingInterface* InterfaceRef = Cast<IShelvingInterface>(HitActor);
        //is Product slot 
        if (InterfaceRef)
        {
            if (GetOwner()->HasAuthority())
            {
                // here is should be Server/Client RPCs 
                InterfaceRef->RemoveFromShelf(CarriedBox, GetOwner());
            }
            else
            {
                Server_RemoveFromShelf(HitActor, CarriedBox);
            }
        }
    }
}

void UShelvingComponent::OnGrabBox()
{
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, "Grab Called");
    AActor* HitActor = TraceForCamera();

    if (HitActor) 
    {
        AProductCrate* BoxToGrab = Cast<AProductCrate>(HitActor);

        if (GetOwner()->HasAuthority()) 
        {
            HandleGrabbingBox(BoxToGrab);
        }
        else 
        {
            Server_HandleGrabbing(BoxToGrab);
        }
    }
}
void UShelvingComponent::HandleGrabbingBox(AProductCrate* BoxToGrab)
{
    if (!BoxToGrab) return;

    if (IsValid(CarriedBox)) //Detach Box 
    {
        DetachCrate();
      
    }
    else // Attach Box
    {
        // if box to grab is not attached to anything 
        if (!(BoxToGrab->GetAttachParentActor()))
        {
            CarriedBox = BoxToGrab;
        }
        AttachCrate();
    }

   
}
void UShelvingComponent::Server_HandleGrabbing_Implementation(AProductCrate* BoxToGrab)
{
    HandleGrabbingBox(BoxToGrab);
}


void UShelvingComponent::OnDropBox()
{
  
    if (CarriedBox) 
    {
        if (GetOwner()->HasAuthority()) 
        {
            HandleDropping();
        }
        else
        {
            Server_HandleDropping();
        }

    }
    
}

void UShelvingComponent::Server_HandleDropping_Implementation()
{
    HandleDropping();
}


//Drop box 
void UShelvingComponent::HandleDropping()
{
    if (IsValid(CarriedBox)) 
    {
        DetachCrate();
        
    }
}

AActor* UShelvingComponent::TraceForCamera()
{
    if (IsValid(OwnerPawn) && OwnerPawn->IsLocallyControlled())
    {
        UWorld* World = GetWorld();
        if (IsValid(World))
        {
            FHitResult HitResult;


            APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController());
            if (PlayerController)
            {
                APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
                if (CameraManager)
                {
                    FVector StartLoc = CameraManager->GetCameraLocation();
                    FVector EndLoc = CameraManager->GetActorForwardVector() * TraceLenght + StartLoc;
                    World->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECC_Visibility);
                    return HitResult.GetActor();

                }
            }


        }

    }
    return nullptr;
}

void UShelvingComponent::Server_AddToShelf_Implementation(AActor* HitActor, class AProductCrate* CarriedCrate)
{

    IShelvingInterface* InterfaceRef = Cast<IShelvingInterface>(HitActor);

    if (InterfaceRef)
    {
        
        InterfaceRef->AddToShelf(CarriedCrate, HitActor->GetOwner());
    }
}
void UShelvingComponent::Server_RemoveFromShelf_Implementation(AActor* HitActor, AProductCrate* CarriedCrate)
{
    IShelvingInterface* InterfaceRef = Cast<IShelvingInterface>(HitActor);

    if (InterfaceRef)
    {

        InterfaceRef->RemoveFromShelf(CarriedCrate, HitActor->GetOwner());
    }
}
void UShelvingComponent::OnRep_CarriedBox()
{
    if (IsValid(CarriedBox)) //Detach Box 
    {
        if (IsValid(BoxMesh)) 
        {
            DetachCrate();
        }
    }
    else // Attach Box
    {
        AttachCrate();
    }

}


bool UShelvingComponent::AttachCrate()
{
    
    BoxMesh = CarriedBox->BoxMesh;

    if (!BoxMesh) return false;

    BoxMesh->SetSimulatePhysics(false);
    BoxMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    UCameraComponent* CameraComp = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
    if (CameraComp && CarriedBox)
    {
        CarriedBox->AttachToComponent(CameraComp, FAttachmentTransformRules::SnapToTargetIncludingScale);
        CarriedBox->SetActorRelativeTransform(GrabbedBoxTransform);

        return true;
    }
  
    return false;
}

bool UShelvingComponent::DetachCrate()
{
    if (!BoxMesh) return false;

    BoxMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxMesh->SetSimulatePhysics(true);
    BoxMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    CarriedBox = nullptr;

    return true;
}

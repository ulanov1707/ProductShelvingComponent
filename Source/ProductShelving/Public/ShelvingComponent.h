// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EProductTypes.h"
#include "ShelvingComponent.generated.h"


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PRODUCTSHELVING_API UShelvingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShelvingComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_AddToShelf(AActor* HitActor, class AProductCrate* CarriedCrate);

	UFUNCTION(Server, Reliable)
	void Server_RemoveFromShelf(AActor* HitActor, class AProductCrate* CarriedCrate);

	UFUNCTION(Client, Reliable)
	void Client_Notify(EProductNotification Notification);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetupInput(class UInputComponent* PlayerInputComponent);
	void OnAddLazy();
	void OnRemoveLazy();
	void OnAddSingle();
	void OnRemoveSigle();
	void OnGrabBox();
	void OnDropBox();

	AActor* TraceForCamera();

	void HandleGrabbingBox(class AProductCrate* BoxToGrab);

	UFUNCTION(Server,Reliable)
	void Server_HandleGrabbing(class AProductCrate* BoxToGrab);

	UFUNCTION(Server, Reliable)
	void Server_HandleDropping();

	void HandleDropping();//Drop box

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	bool AttachCrate();
	bool DetachCrate();

	UPROPERTY(EditAnywhere, Category = "Shelving", meta = (AllowPrivateAccess = "true"))
	float TraceLenght = 300.f;

	UPROPERTY(ReplicatedUsing = OnRep_CarriedBox)
	class AProductCrate* CarriedBox;

	UFUNCTION()
	void OnRep_CarriedBox();
	
	UPROPERTY()
	APawn* OwnerPawn;

	UPROPERTY()
	UStaticMeshComponent* BoxMesh;

	UPROPERTY(EditAnywhere, Category = "Shelving", meta = (AllowPrivateAccess = "true"))
	FTransform GrabbedBoxTransform = FTransform(FRotator(-25.f, 0.f, -90.f), FVector(80.f, 0.f, -35.f), FVector(1.f, 1.f, 1.f));
};

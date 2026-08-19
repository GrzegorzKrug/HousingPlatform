#pragma once


#include "CoreMinimal.h"


#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"


#include "ShowcasePawn.generated.h"


UCLASS(BlueprintType)
class HOMESHOWER_API AShowcasePawn : public APawn {
	GENERATED_BODY()


public:
	AShowcasePawn();


	virtual void BeginPlay() override;

	virtual void Tick( float DeltaTime ) override;

	virtual void SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent ) override;
	
	virtual void PossessedBy(AController* NewController) override;

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;


public:
	UFUNCTION(BlueprintCallable)
	void RefreshCamera();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Yaw = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Pitch = -30;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CameraZoom = 0.1;
	
	// UPROPERTY(BlueprintReadWrite, EditAnywhere)
	// FVector2f CameraLimits = {300, 1500};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotationSpeed = 250;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ZoomSpeed = 0.1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UCurveFloat> CameraCurve;

	UFUNCTION(BlueprintCallable)
	void YawLeft();

	UFUNCTION(BlueprintCallable)
	void YawRight();

	/* Look UP */
	UFUNCTION(BlueprintCallable)
	void PitchDown();

	/* Look Down */
	UFUNCTION(BlueprintCallable)
	void PitchUp();

	UFUNCTION(BlueprintCallable)
	void ZoomIn();

	UFUNCTION(BlueprintCallable)
	void ZoomOut();
	
	UFUNCTION(BlueprintCallable)
	bool GetCursorDirection(FVector& Location,FVector& Direciton);
};

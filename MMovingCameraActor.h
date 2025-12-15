#pragma once

#include "CoreMinimal.h"
#include "Camera/MBaseCameraActor.h"
#include "MovingCameraActor.generated.h"

/**
 * 
 */

UENUM()
enum class EMMovingCamMode : uint8
{
	None = 0,
	NpcTalk = 1
};

UCLASS()
class AMovingCameraActor : public AActor
{
	GENERATED_BODY()

public:
	AMovingCameraActor();

	virtual void Tick(const float InDeltaSeconds) override;

	void SetNpcTalkTarget(APawn* InTargetNPCPawn, const bool bInReOpen);
	void CloseNpcTalkTarget();

private:
	void ChangeCamMode(APawn* InTargetNPCPawn, const bool bInReOpen);
	void UpdateNpcTalk(const float InDeltaSeconds);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComp{nullptr};
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlayerSpringArmComponent> CameraComp{nullptr};

	UPROPERTY()
	FVector StartLocation = FVector::ZeroVector;
	UPROPERTY()
	FRotator StartRotation = FRotator::ZeroRotator;
	UPROPERTY()
	FVector EndLocation = FVector::ZeroVector;
	UPROPERTY()
	FRotator EndRotation = FRotator::ZeroRotator;

	bool bRestore = false;
	bool bCallDestroyFunction = false;
	bool bNeedAllVisibleUI = false;

};

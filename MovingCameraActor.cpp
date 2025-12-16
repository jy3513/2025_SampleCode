
#include "Camera/MovingCameraActor.h"

#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

AMovingCameraActor::AMovingCameraActor()
{
	if (SpringArmComp)
	{
		SpringArmComp->bDoCollisionTest = false;
		SpringArmComp->TargetArmLength = 500.f;
		SpringArmComp->TargetOffset = FVector(0.f, 0.f, 0.f);
	}
}

void AMovingCameraActor::Tick(const float InDeltaSeconds)
{
	Super::Tick(InDeltaSeconds);

	UpdateNpcTalk(InDeltaSeconds);
}

void AMovingCameraActor::SetNpcTalkTarget(APawn* InTargetNPCPawn, const bool bInReOpen)
{
	if (InTargetNPCPawn == nullptr)
	{
		return;
	}

	bRestore = false;
	bNeedAllVisibleUI = true;

	EndLocation = FVector::ZeroVector;
	EndRotation = FRotator::ZeroRotator;

	if (bInReOpen == false)
	{
		ChangeCamMode(InTargetNPCPawn, bInReOpen);
	}
	else
	{
		//	set camera location, rotation.
		if (CameraComponent)
		{
			CameraComponent->SetRelativeLocationAndRotation(EndLocation, EndRotation);
		}
	}

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);)
	{
		PlayerController->SetViewTarget(this);
	}
}

void AMovingCameraActor::CloseNpcTalkTarget()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController == nullptr)
	{
		return;
	}

	APawn* OwnerPawn = PlayerController->GetPawn();
	if (OwnerPawn == nullptr)
	{
		return;
	}

	APlayerCameraActor* DefaultCameraActor = PlayerController->GetCameraActor();
	if (DefaultCameraActor == nullptr)
	{
		return;
	}

	bRestore = true;
	bCallDestroyFunction = false;

	//	set camera location, rotation.
	if (CameraComponent)
	{
		StartLocation = CameraComponent->GetComponentLocation();
		StartRotation = CameraComponent->GetComponentRotation();

		//	attach.
		AttachToActor(OwnerPawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		SetRotation(DefaultCameraActor->GetRotation());

		CameraComponent->SetWorldLocationAndRotation(StartLocation, StartRotation);
	}

	PlayerController->SetLockMoveInput(false);
	PlayerController->SetLockCameraInput(false);
	PlayerController->SetLockSkillInput(false);
}

void AMovingCameraActor::ChangeCamMode(APawn* InTargetNPCPawn, const bool bInReOpen)
{
	if (!InTargetNPCPawn)
	{
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController == nullptr)
	{
		return;
	}

	const APlayerCameraActor* DefaultCameraActor = PlayerController->GetCameraActor();
	if (DefaultCameraActor == nullptr)
	{
		return;
	}

	const UCameraComponent* DefaultCameraComponent = DefaultCameraActor->GetCameraComponent();
	if (DefaultCameraComponent == nullptr)
	{
		return;
	}

	//	change mode.
	const int CameraTypeIndex = GetCameraIndexFromMovingCamEnum(EMovingCamMode::NpcTalk);
	LoadCameraMode(CameraTypeIndex, PlayerController->GetMoveCamSetting(EMovingCamMode::NpcTalk));
	OnSwitchCameraMode(CameraTypeIndex);

	//	attach.
	AttachToActor(InTargetNPCPawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	//	set camera setting.
	const FRotator ForwardRotation = FRotator(0.0f, InTargetNPCPawn->GetActorRotation().Yaw + 180.0f, 0.0f);
	SetRotation(ForwardRotation);

	//	set camera location, rotation.
	if (CameraComponent)
	{
		CameraComponent->SetWorldLocationAndRotation(DefaultCameraComponent->GetComponentLocation(), DefaultCameraComponent->GetComponentRotation());
	}
}

void AMovingCameraActor::UpdateNpcTalk(const float InDeltaSeconds)
{
	if (CameraComponent == nullptr || CurCameraSetting.IsValid() == false)
	{
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController == nullptr)
	{
		return;
	}

	const float InterpSpeed = CurCameraSetting->GetInterpSpeed();

	if (bRestore == false)
	{
		if (FMath::IsNearlyZero(CurCameraSetting->GetCurZoomValue()) == false)
		{
			InputZoom(-0.5f);
		}

		const FVector ResultLocation = FMath::VInterpTo(CameraComponent->GetRelativeLocation(), EndLocation, InDeltaSeconds, InterpSpeed);
		const FRotator ResultRotation = FMath::RInterpTo(CameraComponent->GetRelativeRotation(), EndRotation, InDeltaSeconds, InterpSpeed);

		if (EndLocation.Equals(CameraComponent->GetRelativeLocation(), 0.01f) == false && EndRotation.Equals(CameraComponent->GetRelativeRotation(), 0.01f) == false)
		{
			CameraComponent->SetRelativeLocationAndRotation(ResultLocation, ResultRotation);
		}

		if (bNeedAllVisibleUI)
		{
			if (EndLocation.Equals(ResultLocation, 50.f))
			{
				bNeedAllVisibleUI = false;
			}
		}
	}
	else
	{
		if (FMath::IsNearlyEqual(CurCameraSetting->GetCurZoomValue(), 1.0f) == false)
		{
			InputZoom(0.5f);
		}

		const APlayerCameraActor* DefaultCameraActor = PlayerController->GetCameraActor();
		if (DefaultCameraActor == nullptr)
		{
			return;
		}

		const UCameraComponent* DefaultCameraComponent = DefaultCameraActor->GetCameraComponent();
		if (DefaultCameraComponent == nullptr)
		{
			return;
		}

		SetRotation(DefaultCameraActor->GetRotation());

		EndLocation = DefaultCameraComponent->GetComponentLocation();

		const FVector ResultLocation = FMath::VInterpTo(CameraComponent->GetComponentLocation(), EndLocation, InDeltaSeconds, InterpSpeed);
		const FRotator ResultRotation = FMath::RInterpTo(CameraComponent->GetComponentRotation(), DefaultCameraComponent->GetComponentRotation(), InDeltaSeconds, InterpSpeed);

		CameraComponent->SetWorldLocationAndRotation(ResultLocation, ResultRotation);

		if (bCallDestroyFunction == false)
		{
			if (EndLocation.Equals(ResultLocation, 1.0f))
			{
				bCallDestroyFunction = true;

				if (PlayerController->GetViewTarget() == this)
				{
					PlayerController->RestoreViewTarget();
				}

				Destroy();
			}
		}
	}
}

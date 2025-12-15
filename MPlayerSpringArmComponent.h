#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerSpringArmComponent.generated.h"

/**
 * 
 */
UCLASS()
class UPlayerSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

protected:
	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;
	
};

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Components/Slider.h"
#include "DoubleHandleSlider.generated.h"

/**
 *
 */

class SDoubleHandleSlider;

UCLASS()
class UDoubleHandleSlider : public USlider
{
	GENERATED_BODY()

public:
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	float GetFirstValue() const;
	void SetFirstValue(float InValue);
	float GetSecondValue() const;
	void SetSecondValue(float InValue);

	void SetStepSize(const float InStepSize);
	void SetMinAndMaxValues(const float InMinValue, const float InMaxValue);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (UIMin = "0", UIMax = "1"))
		float SecondValue;

	UPROPERTY()
		FGetFloat SecondValueDelegate;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		FSlateBrush FillImage;

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDoubleHandleValueChangedEvent, float, FirstValue, float, SecondValue);
	FOnDoubleHandleValueChangedEvent OnDoubleHandleValueChangedEvent;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	void HandleOnValueChanged(float InFirstValue, float InSecondValue);

protected:
	TSharedPtr<SDoubleHandleSlider> MyDoubleHandleSlider;

	PROPERTY_BINDING_IMPLEMENTATION(float, SecondValue);

};
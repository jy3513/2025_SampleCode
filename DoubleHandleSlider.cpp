
#include "UI/Common/DoubleHandleSlider.h"

#include "SDoubleHandleSlider.h"

#define LOCTEXT_NAMESPACE "UDoubleHandleSlider"

void UDoubleHandleSlider::SynchronizeProperties()
{
    Super::SynchronizeProperties();

    if (MyDoubleHandleSlider.IsValid() == false)
    {
        return;
    }

    PRAGMA_DISABLE_DEPRECATION_WARNINGS

    TAttribute<float> FirstValueBinding = PROPERTY_BINDING(float, Value);
    TAttribute<float> SecondValueBinding = PROPERTY_BINDING(float, SecondValue);

    MyDoubleHandleSlider->SetFirstValue(FirstValueBinding);
    MyDoubleHandleSlider->SetSecondValue(SecondValueBinding);
    MyDoubleHandleSlider->SetMinAndMaxValues(MinValue, MaxValue);
    MyDoubleHandleSlider->SetStepSize(StepSize);

    PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

void UDoubleHandleSlider::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);

    MyDoubleHandleSlider.Reset();
}

#if WITH_EDITOR
const FText UDoubleHandleSlider::GetPaletteCategory()
{
    return LOCTEXT("CustomWidget", "CustomWidget");
}
#endif

TSharedRef<SWidget> UDoubleHandleSlider::RebuildWidget()
{
    PRAGMA_DISABLE_DEPRECATION_WARNINGS

    MyDoubleHandleSlider = SNew(SDoubleHandleSlider)
    .Style(&WidgetStyle)
    .FillImage(&FillImage)
    .OnDoubleHandleSliderValueChanged(BIND_UOBJECT_DELEGATE(FOnDoubleHandleSliderValueChanged, HandleOnValueChanged));

    PRAGMA_ENABLE_DEPRECATION_WARNINGS

    return MyDoubleHandleSlider.ToSharedRef();
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
float UDoubleHandleSlider::GetFirstValue() const
{
    return GetValue();
}

void UDoubleHandleSlider::SetFirstValue(float InValue)
{
    if (MyDoubleHandleSlider.IsValid())
    {
        MyDoubleHandleSlider->SetValue(InValue);
    }

    if (Value != InValue)
    {
        Value = InValue;
        HandleOnValueChanged(InValue, GetSecondValue());
    }
}

float UDoubleHandleSlider::GetSecondValue() const
{
    if (MyDoubleHandleSlider.IsValid())
    {
        return MyDoubleHandleSlider->GetSecondValue();
    }

    return SecondValue;
}

void UDoubleHandleSlider::SetSecondValue(float InValue)
{
    if (MyDoubleHandleSlider.IsValid())
    {
        MyDoubleHandleSlider->SetValue(InValue);
    }

    if (SecondValue != InValue)
    {
        SecondValue = InValue;
        HandleOnValueChanged(GetFirstValue(), InValue);
    }
}

void UDoubleHandleSlider::SetStepSize(const float InStepSize)
{
    if (MyDoubleHandleSlider.IsValid())
    {
        MyDoubleHandleSlider->SetStepSize(InStepSize);
    }
}

void UDoubleHandleSlider::SetMinAndMaxValues(const float InMinValue, const float InMaxValue)
{
    if (MyDoubleHandleSlider.IsValid())
    {
        MyDoubleHandleSlider->SetMinAndMaxValues(InMinValue, InMaxValue);
    }
}

PRAGMA_ENABLE_DEPRECATION_WARNINGS

void UDoubleHandleSlider::HandleOnValueChanged(float InFirstValue, float InSecondValue)
{
    PRAGMA_DISABLE_DEPRECATION_WARNINGS

    Value = InFirstValue;
    SecondValue = InSecondValue;

    PRAGMA_ENABLE_DEPRECATION_WARNINGS

    OnDoubleHandleValueChangedEvent.Broadcast(InFirstValue, InSecondValue);

    BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::Value);
}

#undef LOCTEXT_NAMESPACE
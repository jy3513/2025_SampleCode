#pragma once

#include "CoreMinimal.h"

#include "Input/Reply.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/SCompoundWidget.h"

 /**
  *
  */

DECLARE_DELEGATE_TwoParams(FOnDoubleHandleSliderValueChanged, float, float);

class SMDoubleHandleSlider : public SSlider
{
public:
    SLATE_BEGIN_ARGS(SMDoubleHandleSlider)
        :_Style(&FCoreStyle::Get().GetWidgetStyle<FSliderStyle>("Slider"))
        , _FillImage(nullptr)
        , _SliderBarColor(FLinearColor::White)
        , _SliderFillColor(FLinearColor::White)
        , _SliderHandleColor(FLinearColor::White)
        , _StepSize(0.0f)
        , _MinValue(0.0f)
        , _MaxValue(0.0f)
        , _FirstValue(0.0f)
        , _SecondValue(0.0f)
        , _OnDoubleHandleSliderValueChanged()
    {}

    SLATE_STYLE_ARGUMENT(FSliderStyle, Style)

        SLATE_ARGUMENT(const FSlateBrush*, FillImage)

        SLATE_ATTRIBUTE(FSlateColor, SliderBarColor)

        SLATE_ATTRIBUTE(FSlateColor, SliderFillColor)

        SLATE_ATTRIBUTE(FSlateColor, SliderHandleColor)

        SLATE_ATTRIBUTE(float, StepSize)

        SLATE_ARGUMENT(float, MinValue)

        SLATE_ARGUMENT(float, MaxValue)

        SLATE_ATTRIBUTE(float, FirstValue)

        SLATE_ATTRIBUTE(float, SecondValue)

        SLATE_EVENT(FOnDoubleHandleSliderValueChanged, OnDoubleHandleSliderValueChanged)

        SLATE_END_ARGS()

        SMDoubleHandleSlider();

    void Construct(const FArguments& InArgs);

public:
    int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;

    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual void OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
    virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
    virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
    virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent) override;

public:
    float GetFirstValue() const;
    float GetSecondValue() const;

    float GetNormalizedFirstValue() const;
    float GetNormalizedSecondValue() const;

    void SetFirstValue(TAttribute<float> InValueAttribute);
    void SetSecondValue(TAttribute<float> InValueAttribute);

protected:
    virtual void CommitValue(float InNewValue) override;

    const FSlateBrush* GetFillImage() const;

private:
    void ResetHandle();
    void CheckHandle(const FGeometry& MyGeometry, const UE::Slate::FDeprecateVector2DParameter& AbsolutePosition);

private:
    bool IsFirstHandle = false;
    bool IsSecondHandle = false;

    float SecondValue;

    const FSlateBrush* FillImage;

    FSlateColor SliderBarColor;
    FSlateColor SliderFillColor;
    FSlateColor SliderHandleColor;

    FOnDoubleHandleSliderValueChanged OnDoubleHandleSliderValueChanged;

};
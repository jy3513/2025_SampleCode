
#include "UI/Common/SDoubleHandleSlider.h"

SDoubleHandleSlider::SDoubleHandleSlider()
   : SecondValue(1.f)
   , FillImage(nullptr)
   , SliderBarColor(FLinearColor::White)
   , SliderFillColor(FLinearColor::White)
   , SliderHandleColor(FLinearColor::White)
{}

void SDoubleHandleSlider::Construct(const FArguments& InArgs)
{
   Style = InArgs._Style;

   FillImage = InArgs._FillImage;
   
   SliderBarColor = InArgs._SliderBarColor.Get();
   SliderFillColor = InArgs._SliderFillColor.Get();
   SliderHandleColor = InArgs._SliderHandleColor.Get();

   MinValue = InArgs._MinValue;
   MaxValue = InArgs._MaxValue;

   StepSize = InArgs._StepSize;
   bMouseUsesStep = false;

   Orientation = EOrientation::Orient_Horizontal;

   SetFirstValue(InArgs._FirstValue);
   SetSecondValue(InArgs._SecondValue);

   OnDoubleHandleSliderValueChanged = InArgs._OnDoubleHandleSliderValueChanged;
}

int32 SDoubleHandleSlider::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
   const float AllottedWidth = AllottedGeometry.GetLocalSize().X;
   const float AllottedHeight = AllottedGeometry.GetLocalSize().Y;

   // calculate slider geometry as if it's a horizontal slider (we'll rotate it later if it's vertical)
   const FVector2f HandleSize = GetThumbImage()->ImageSize;
   const FVector2f HalfHandleSize = 0.5f * HandleSize;

   // We clamp to make sure that the slider cannot go out of the slider Length.
   const float SliderLength = AllottedWidth - HandleSize.X;
   const float SliderFirstHandleOffset = FMath::Clamp(GetNormalizedFirstValue(), 0.0f, 1.0f) * SliderLength;
   const float SliderSecondHandleOffset = FMath::Clamp(GetNormalizedSecondValue(), 0.0f, 1.0f) * SliderLength;
   const float SliderY = 0.5f * AllottedHeight;

   const FVector2f SliderStartPoint = FVector2f(HalfHandleSize.X, SliderY);
   const FVector2f SliderEndPoint = FVector2f(AllottedWidth - HalfHandleSize.X, SliderY);

   // draw slider bar
   const FVector2f FirstHandleTopLeftPoint = FVector2f(SliderFirstHandleOffset, SliderY - HalfHandleSize.Y);
   const FVector2f SecondHandleTopLeftPoint = FVector2f(SliderSecondHandleOffset, SliderY - HalfHandleSize.Y);
   const FVector2f BarTopLeft = FVector2f(SliderStartPoint.X, SliderStartPoint.Y - Style->BarThickness * 0.5f);
   const FVector2f FillTopLeft = FVector2f(FMath::Min(SliderFirstHandleOffset, SliderSecondHandleOffset), SliderStartPoint.Y - Style->BarThickness * 0.5f);

   const FVector2f BarSize = FVector2f(SliderEndPoint.X - SliderStartPoint.X, Style->BarThickness);
   const FVector2f FillSize = FVector2f(FMath::Max(SliderFirstHandleOffset, SliderSecondHandleOffset) - FillTopLeft.X + HalfHandleSize.X, Style->BarThickness);

   const FSlateBrush* BarImage = GetBarImage();
   const FSlateBrush* ThumbImage = GetThumbImage();
   const FSlateBrush* CurrentFillImage = GetFillImage();

   const ESlateDrawEffect DrawEffects = ShouldBeEnabled(bParentEnabled) ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

   FSlateDrawElement::MakeBox(
      OutDrawElements,
      LayerId,
      AllottedGeometry.ToPaintGeometry(BarSize, FSlateLayoutTransform(BarTopLeft)),
      BarImage,
      DrawEffects,
      BarImage->GetTint(InWidgetStyle) * SliderBarColor.GetColor(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
   );

   ++LayerId;

   //   draw slider fill
   FSlateDrawElement::MakeBox(
      OutDrawElements,
      LayerId,
      AllottedGeometry.ToPaintGeometry(FillSize, FSlateLayoutTransform(FillTopLeft)),
      CurrentFillImage,
      DrawEffects,
      CurrentFillImage->GetTint(InWidgetStyle) * SliderFillColor.GetColor(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
   );

   ++LayerId;

   // draw slider thumb
   FSlateDrawElement::MakeBox(
      OutDrawElements,
      LayerId,
      AllottedGeometry.ToPaintGeometry(HandleSize, FSlateLayoutTransform(FirstHandleTopLeftPoint)),
      ThumbImage,
      DrawEffects,
      ThumbImage->GetTint(InWidgetStyle) * SliderHandleColor.GetColor(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
   );

   ++LayerId;

   // draw slider thumb
   FSlateDrawElement::MakeBox(
      OutDrawElements,
      LayerId,
      AllottedGeometry.ToPaintGeometry(HandleSize, FSlateLayoutTransform(SecondHandleTopLeftPoint)),
      ThumbImage,
      DrawEffects,
      ThumbImage->GetTint(InWidgetStyle) * SliderHandleColor.GetColor(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
   );

   return LayerId;
}

FReply SDoubleHandleSlider::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
   if ((MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) && !IsLocked())
   {
      CheckHandle(MyGeometry, MouseEvent.GetLastScreenSpacePosition());
   }

   return SSlider::OnMouseButtonDown(MyGeometry, MouseEvent);
}

FReply SDoubleHandleSlider::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
   ResetHandle();

   return SSlider::OnMouseButtonUp(MyGeometry, MouseEvent);
}

void SDoubleHandleSlider::OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent)
{
   ResetHandle();

   return SSlider::OnMouseCaptureLost(CaptureLostEvent);
}

FReply SDoubleHandleSlider::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
   if (HasMouseCaptureByUser(MouseEvent.GetUserIndex(), MouseEvent.GetPointerIndex()) && !IsLocked())
   {
      CheckHandle(MyGeometry, MouseEvent.GetLastScreenSpacePosition());
   }

   return SSlider::OnMouseMove(MyGeometry, MouseEvent);
}

FReply SDoubleHandleSlider::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
   if (HasMouseCaptureByUser(InTouchEvent.GetUserIndex(), InTouchEvent.GetPointerIndex()))
   {
      CheckHandle(MyGeometry, InTouchEvent.GetScreenSpacePosition());
   }
   else if (!HasMouseCapture())
   {
      CheckHandle(MyGeometry, InTouchEvent.GetScreenSpacePosition());
   }

   return SSlider::OnTouchMoved(MyGeometry, InTouchEvent);
}

FReply SDoubleHandleSlider::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
   ResetHandle();

   return SSlider::OnTouchEnded(MyGeometry, InTouchEvent);
}

void SDoubleHandleSlider::OnFocusLost(const FFocusEvent& InFocusEvent)
{
   ResetHandle();

   return SSlider::OnFocusLost(InFocusEvent);
}

FNavigationReply SDoubleHandleSlider::OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent)
{
   return FNavigationReply::Escape();
}

float SDoubleHandleSlider::GetFirstValue() const
{
   return GetValue();
}

float SDoubleHandleSlider::GetSecondValue() const
{
   return SecondValue;
}

float SDoubleHandleSlider::GetNormalizedFirstValue() const
{
   return GetNormalizedValue();
}

float SDoubleHandleSlider::GetNormalizedSecondValue() const
{
   if (MaxValue == MinValue)
   {
      return 1.0f;
   }
   else
   {
      return (SecondValue - MinValue) / (MaxValue - MinValue);
   }
}

void SDoubleHandleSlider::SetFirstValue(TAttribute<float> InValueAttribute)
{
   SetValue(InValueAttribute);
}

void SDoubleHandleSlider::SetSecondValue(TAttribute<float> InValueAttribute)
{
   SecondValue = InValueAttribute.Get();
}

void SDoubleHandleSlider::CommitValue(float InNewValue)
{
   if (IsFirstHandle == false && IsSecondHandle == false)
   {
      return;
   }

   float NewValue = InNewValue;
   float CheckValue = IsFirstHandle ? GetFirstValue() : GetSecondValue();

   float Direction = CheckValue - InNewValue;
   float CurrentStepSize = StepSize.Get();
   if (Direction > CurrentStepSize / 2.0f)
   {
      NewValue = FMath::Clamp(CheckValue - CurrentStepSize, MinValue, MaxValue);
   }
   else if (Direction < CurrentStepSize / -2.0f)
   {
      NewValue = FMath::Clamp(CheckValue + CurrentStepSize, MinValue, MaxValue);
   }
   else
   {
      NewValue = CheckValue;
   }

   if (IsFirstHandle)
   {
      SetFirstValue(NewValue);
   }
   else
   {
      SetSecondValue(NewValue);
   }

   Invalidate(EInvalidateWidgetReason::Paint);

   OnDoubleHandleSliderValueChanged.ExecuteIfBound(GetFirstValue(), GetSecondValue());
}

const FSlateBrush* SDoubleHandleSlider::GetFillImage() const
{
   return FillImage ? FillImage : &Style->NormalBarImage;
}

void SDoubleHandleSlider::ResetHandle()
{
   IsFirstHandle = false;
   IsSecondHandle = false;
}

void SDoubleHandleSlider::CheckHandle(const FGeometry& MyGeometry, const UE::Slate::FDeprecateVector2DParameter& AbsolutePosition)
{
   if (IsFirstHandle || IsSecondHandle)
   {
      return;
   }

   const FVector2f LocalPosition = MyGeometry.AbsoluteToLocal(AbsolutePosition);

   const float CheckFirstValue = GetFirstValue();
   const float CheckSecondValue = GetSecondValue();

   // Only need X as we rotate the thumb image when rendering vertically
   const float Indentation = GetThumbImage()->ImageSize.X;
   const float HalfIndentation = 0.5f * Indentation;

   float Denominator = Denominator = MyGeometry.Size.X - Indentation;
   float RelativeValue = RelativeValue = (Denominator != 0.f) ? (LocalPosition.X - HalfIndentation) / Denominator : 0.f;
      
   RelativeValue = FMath::Clamp(RelativeValue, 0.0f, 1.0f) * (GetMaxValue() - GetMinValue()) + GetMinValue();

   if (FMath::Abs(RelativeValue - CheckFirstValue) < FMath::Abs(RelativeValue - CheckSecondValue))
   {
      IsFirstHandle = true;
   }
   else
   {
      IsSecondHandle = true;
   }
}

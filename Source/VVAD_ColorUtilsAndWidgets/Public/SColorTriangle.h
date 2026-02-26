#pragma once

#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"
#include "Styling/SlateBrush.h"

DECLARE_DELEGATE_OneParam(FOnTriangleXYChangedNative, FVector2D /*XY*/);
DECLARE_DELEGATE_OneParam(FOnRingChangedNative, float /*angle*/);

class SColorTriangle : public SLeafWidget {
public:
	SLATE_BEGIN_ARGS(SColorTriangle) {}
		SLATE_ATTRIBUTE(FVector2D, XY)
		SLATE_ATTRIBUTE(float, RingValue)
		SLATE_EVENT(FOnTriangleXYChangedNative, OnXYChanged)
		SLATE_EVENT(FOnRingChangedNative, OnRingChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs) {
		XYAttr = InArgs._XY;
		RingValue = InArgs._RingValue;
		OnXYChanged = InArgs._OnXYChanged;
		OnRingChanged = InArgs._OnRingChanged;
	}

	void SetBackgroundBrush(const FSlateBrush* InBrush) { BackgroundBrush = InBrush; }
	void SetKnobBrush(const FSlateBrush* InBrush) { KnobBrush = InBrush; }
	void SetRingKnobBrush(const FSlateBrush* InBrush) { RingKnobBrush = InBrush; }
	void SetRotateRingKnob(bool RotateRingKnob) { bRotateRingKnob = RotateRingKnob; }
	void SetRadius(float radius) { RingRadius = radius; }

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
		int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override
	{
		const bool bEnabled = ShouldBeEnabled(bParentEnabled);
		const ESlateDrawEffect Effects = bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

		// Background
		if (BackgroundBrush) {
			FSlateDrawElement::MakeBox(
				OutDrawElements, LayerId,
				AllottedGeometry.ToPaintGeometry(),
				BackgroundBrush, Effects,
				InWidgetStyle.GetColorAndOpacityTint()
			);
		}

		// Knob
		if (KnobBrush) {
			const FVector2D Size = AllottedGeometry.GetLocalSize();
			const float X = FMath::Clamp(XYAttr.Get(FVector2D(1,1)).X, 0.f, 1.f);
			const float Y = FMath::Clamp(XYAttr.Get(FVector2D(1, 1)).Y, 0.f, 1.f);

			const FVector2D KnobSize =  KnobBrush->GetImageSize();

			const FVector2D P(
				X * Size.X,
				(1.f - Y) * Size.Y
			);

			const FVector2D KnobTopLeft = P - (KnobSize * 0.5f);

			FSlateDrawElement::MakeBox(
				OutDrawElements, LayerId + 1,
				AllottedGeometry.ToPaintGeometry(KnobTopLeft, KnobSize),
				KnobBrush, Effects,
				KnobBrush->GetTint(InWidgetStyle)
				//InWidgetStyle.GetColorAndOpacityTint()
			);
		}

		// RingKnob
		if (RingKnobBrush) {
			const FVector2D Size = AllottedGeometry.GetLocalSize();
			const FVector2D Center = Size * 0.5f;

			const float RingValueDeg = FMath::Fmod(RingValue.Get(0), 360.f);
			const float RingRadius01 = FMath::Clamp(RingRadius, 0.f, 1.f);
			FVector2D KnobSize = RingKnobBrush->GetImageSize();

			const float dist = RingRadius01 + ((1.f - RingRadius01) / 2.f);

			FVector2D target = FVector2D(dist*Center.X - (KnobSize.X / 2.f), 0);
			target = target.GetRotated(RingValueDeg);
			FVector2D offs = target.GetRotated(-90).GetSafeNormal()*(KnobSize.Y/2.f);
			target += Center + offs;

			FSlateDrawElement::MakeRotatedBox(
				OutDrawElements,
				LayerId + 1,
				AllottedGeometry.ToPaintGeometry(target, KnobSize),
				RingKnobBrush,
				Effects,
				FMath::DegreesToRadians(RingValueDeg),
				FVector2D(.5f,.5f),
				FSlateDrawElement::RelativeToElement,
				RingKnobBrush->GetTint(InWidgetStyle)
			);
		}

		return LayerId + 2;
	}

	virtual FVector2D ComputeDesiredSize(float) const override {
		return FVector2D(150.f, 150.f);
	}

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override {
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) {
			bIsCenterInteraction = IsCenter(MyGeometry, MouseEvent);
			bDragging = true;
			UpdateFromPointer(MyGeometry, MouseEvent);
			return FReply::Handled().CaptureMouse(AsShared());
		}
		return FReply::Unhandled();
	}

	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override {
		if (bDragging) {
			UpdateFromPointer(MyGeometry, MouseEvent);
			return FReply::Handled();
		}
		return FReply::Unhandled();
	}

	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override {
		if (bDragging && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) {
			bDragging = false;
			UpdateFromPointer(MyGeometry, MouseEvent);
			return FReply::Handled().ReleaseMouseCapture();
		}
		return FReply::Unhandled();
	}

private:
	void UpdateFromPointer(const FGeometry& MyGeometry, const FPointerEvent& E) {
		const FVector2D Local = MyGeometry.AbsoluteToLocal(E.GetScreenSpacePosition());
		const FVector2D Size = MyGeometry.GetLocalSize();

		FVector2D newXY;
		newXY = ClampToCircle(FVector2D(Local.X / Size.X, 1.f - (Local.Y / Size.Y))) * 2.f;
		
		if (bIsCenterInteraction) {
			OnXYChanged.ExecuteIfBound(newXY);
		} else {
			float angle = 360.0f -FMath::RadiansToDegrees(FMath::Atan2(newXY.Y, newXY.X));
			angle = FMath::Fmod(angle + 360.0f, 360.0f);
			OnRingChanged.ExecuteIfBound(angle);
		}
	}

	FVector2D ClampToCircle(FVector2D in) {
		const FVector2D half = FVector2D(.5, .5);
		const FVector2D centered = (in - half);

		float len = centered.Size();
		if (len <= .5f) return centered;

		len = .5f;
		FVector2D n = centered.GetSafeNormal();
		return (n * len);
	}

	bool IsCenter(const FGeometry& MyGeometry, const FPointerEvent& E) {
		const FVector2D Local = MyGeometry.AbsoluteToLocal(E.GetScreenSpacePosition());
		const FVector2D Size = MyGeometry.GetLocalSize();

		FVector2D in;
		const float X = (Size.X > 0.f) ? FMath::Clamp(Local.X / Size.X, 0.f, 1.f) : 0.f;
		const float Y = (Size.Y > 0.f) ? FMath::Clamp(1.f - (Local.Y / Size.Y), 0.f, 1.f) : 0.f;
		in = FVector2D(X, Y);

		const FVector2D half = FVector2D(.5, .5);
		float len = (in - half).Size()*2.f;
		return (len <= RingRadius);
	}

	TAttribute<FVector2D> XYAttr;
	TAttribute<float> RingValue;
	FOnTriangleXYChangedNative OnXYChanged;
	FOnRingChangedNative OnRingChanged;

	const FSlateBrush* BackgroundBrush = nullptr;
	const FSlateBrush* KnobBrush = nullptr;
	const FSlateBrush* RingKnobBrush = nullptr;

	bool bRotateRingKnob = false;
	float RingRadius = 0.7f;

	bool bIsCenterInteraction = false;
	bool bDragging = false;
};

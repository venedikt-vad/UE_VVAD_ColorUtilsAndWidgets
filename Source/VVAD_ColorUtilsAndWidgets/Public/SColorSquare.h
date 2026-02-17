#pragma once

#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"
#include "Styling/SlateBrush.h"

DECLARE_DELEGATE_OneParam(FOnXYChangedNative, FVector2D /*XY*/);

class SColorSquare : public SLeafWidget {
public:
	SLATE_BEGIN_ARGS(SColorSquare) {}
		SLATE_ATTRIBUTE(FVector2D, XY)
		SLATE_EVENT(FOnXYChangedNative, OnXYChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs) {
		XYAttr = InArgs._XY;
		OnXYChanged = InArgs._OnXYChanged;
	}

	void SetBackgroundBrush(const FSlateBrush* InBrush) { BackgroundBrush = InBrush; }
	void SetKnobBrush(const FSlateBrush* InBrush) { KnobBrush = InBrush; }

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

		return LayerId + 2;
	}

	virtual FVector2D ComputeDesiredSize(float) const override {
		return FVector2D(150.f, 150.f);
	}

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override {
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) {
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

		const float X = (Size.X > 0.f) ? FMath::Clamp(Local.X / Size.X, 0.f, 1.f) : 0.f;
		const float Y = (Size.Y > 0.f) ? FMath::Clamp(1.f - (Local.Y / Size.Y), 0.f, 1.f) : 0.f;

		OnXYChanged.ExecuteIfBound(FVector2D(X, Y));
	}

	TAttribute<FVector2D> XYAttr;
	FOnXYChangedNative OnXYChanged;

	const FSlateBrush* BackgroundBrush = nullptr;
	const FSlateBrush* KnobBrush = nullptr;

	bool bDragging = false;
};

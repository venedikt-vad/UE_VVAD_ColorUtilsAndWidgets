#include "ColorTriangle.h"
#include "SColorTriangle.h"

UColorTriangle::UColorTriangle(const FObjectInitializer& ObjectInitializer) {
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HS_MatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/HSV/M_UI_HSVTriangle.M_UI_HSVTriangle'"));
	if (HS_MatFinder.Succeeded() && HS_MatFinder.Object) {
		HS_Mat = HS_MatFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ThumbMatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/M_UI_Thumb.M_UI_Thumb'"));
	if (ThumbMatFinder.Succeeded() && ThumbMatFinder.Object) {
		UMaterialInterface* ThumbMat = ThumbMatFinder.Object;

		ApplyMatBrush(KnobBrush, FVector2D(10, 10), ThumbMat);
		//ApplyMatBrush(KnobBrushHovered, FVector2D(10, 10), ThumbMat);
		//ApplyMatBrush(KnobBrushDisabled, FVector2D(10, 10), ThumbMat);

		ApplyMatBrush(DarkKnobBrush, FVector2D(10, 10), ThumbMat, FLinearColor::Black);
		//ApplyMatBrush(DarkKnobBrushHovered, FVector2D(10, 10), ThumbMat, FLinearColor::Black);
		//ApplyMatBrush(DarkKnobBrushDisabled, FVector2D(10, 10), ThumbMat, FLinearColor::Black);
	}
}


TSharedRef<SWidget> UColorTriangle::RebuildWidget() {
	MyXYSquare = SNew(SColorTriangle)
		.XY(TAttribute<FVector2D>::Create(TAttribute<FVector2D>::FGetter::CreateUObject(this, &UColorTriangle::GetXY)))
		.OnXYChanged(FOnXYChangedNative::CreateLambda([this](FVector2D XY) {
		
			CurrentPos = ClampToCircle(XY);

			OnXYChanged.Broadcast(CurrentPos);
		
			FLinearColor NewColor = CurrentValueHSV;
			
			{
				const FVector2D Center(0.5f, 0.5f);
				const FVector2D OffsetPos = CurrentPos - Center;
				NewColor.R = 360.0f -HueOffset -FMath::RadiansToDegrees(FMath::Atan2(OffsetPos.Y, OffsetPos.X));
				NewColor.R = FMath::Fmod(NewColor.R + 360.0f, 360.0f);

				NewColor.G = FMath::Clamp(OffsetPos.Size() * 2.0f, 0.0f, 1.0f);
			}

			CurrentValueHSV = NewColor;
			
			OnColorChanged.Broadcast(NewColor);

		}));

	return MyXYSquare.ToSharedRef();
}

void UColorTriangle::ReleaseSlateResources(bool bReleaseChildren) {
	Super::ReleaseSlateResources(bReleaseChildren);
	MyXYSquare.Reset();
}


void UColorTriangle::EnsureMID() {
	BackgroundMaterial = HS_Mat;

	if (!BackgroundMaterial) { BackgroundMID = nullptr; return; }

	if (!BackgroundMID || BackgroundMID->Parent != BackgroundMaterial) {
		BackgroundMID = UMaterialInstanceDynamic::Create(BackgroundMaterial, this);
		BackgroundBrush = FSlateBrush();
		BackgroundBrush.DrawAs = ESlateBrushDrawType::Image;
		BackgroundBrush.SetResourceObject(BackgroundMID);
		BackgroundBrush.ImageSize = FVector2D(256.f, 256.f);
	}
}

void UColorTriangle::UpdateMID() {
	EnsureMID();
	if (BackgroundMID) {
		BackgroundMID->SetScalarParameterValue(TEXT("Hue"), CurrentValueHSV.R / 360.f);
		BackgroundMID->SetScalarParameterValue(TEXT("Saturation"), CurrentValueHSV.G);
		BackgroundMID->SetScalarParameterValue(TEXT("Value"), CurrentValueHSV.B);
		BackgroundMID->SetScalarParameterValue(TEXT("HueOffset"), -HueOffset / 360.f);
	}
	if (MyXYSquare.IsValid()) {
		MyXYSquare->SetIsCircle(true);
		MyXYSquare->SetBackgroundBrush(BackgroundMID ? &BackgroundBrush : nullptr);
		if (CurrentValueHSV.B > 0.5 && bUseDarkKnobOnLightSurface) {
			MyXYSquare->SetKnobBrush(&DarkKnobBrush);
		} else {
			MyXYSquare->SetKnobBrush(&KnobBrush);

		}
	}
}

FVector2D UColorTriangle::ClampToCircle(FVector2D in) {
	const FVector2D half = FVector2D(.5, .5);

	float len = (in - half).Size();
	if (len <= .5f) return in;

	len = .5f;
	FVector2D n = (in - half).GetSafeNormal();
	return (n * len) + half;
}

void UColorTriangle::SynchronizeProperties() {
	Super::SynchronizeProperties();
	UpdateMID();
}

void UColorTriangle::SetXY(FVector2D newValue) {
	CurrentPos = ClampToCircle(newValue);

	if (MyXYSquare.IsValid()) {
		MyXYSquare->Invalidate(EInvalidateWidget::Paint);
	}
}

void UColorTriangle::SetColor(FLinearColor NewColor) {
	FLinearColor colNew = NewColor.LinearRGBToHSV();
	if ((colNew.G == 0 || colNew.B == 0) && colNew.R == 0) {
		colNew.R = CurrentValueHSV.R;
	}
	SetColorHSV(colNew);
}

void UColorTriangle::SetColorHSV(FLinearColor NewColorHSV) {
	CurrentValueHSV = NewColorHSV;

	FVector2D target = FVector2D(NewColorHSV.G / 2.f, 0);
	target = target.GetRotated(-NewColorHSV.R - HueOffset);
	target += FVector2D(.5f, .5f);
	SetXY(target);

	UpdateMID();
}

FLinearColor UColorTriangle::GetColorHSV() {
	return CurrentValueHSV;
}

FLinearColor UColorTriangle::GetColor() {
	return GetColorHSV().HSVToLinearRGB();
}

void UColorTriangle::ApplyMatBrush(FSlateBrush& Brush, const FVector2D& Size, UMaterialInterface* Mat, FLinearColor Tint) {
	Brush.SetResourceObject(Mat);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = Size;
	Brush.TintColor = FSlateColor(Tint);
}
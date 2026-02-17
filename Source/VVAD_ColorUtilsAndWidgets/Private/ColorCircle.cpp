#include "ColorCircle.h"
#include "SColorSquare.h"

UColorCircle::UColorCircle(const FObjectInitializer& ObjectInitializer) {
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HS_MatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/HSV/M_UI_HSCircle.M_UI_HSCircle'"));
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


TSharedRef<SWidget> UColorCircle::RebuildWidget() {
	MyXYSquare = SNew(SColorSquare)
		.XY(TAttribute<FVector2D>::Create(TAttribute<FVector2D>::FGetter::CreateUObject(this, &UColorCircle::GetXY)))
		.OnXYChanged(FOnXYChangedNative::CreateLambda([this](FVector2D XY) {
		
			CurrentPos = ClampToCircle(XY);

			OnXYChanged.Broadcast(CurrentPos);
		
			FLinearColor NewColor = CurrentValueHSV;
			
			{
				const FVector2D Center(0.5f, 0.5f);
				const FVector2D OffsetPos = CurrentPos - Center;
				NewColor.R = - FMath::RadiansToDegrees(FMath::Atan2(OffsetPos.Y, OffsetPos.X));
				NewColor.R = FMath::Fmod(NewColor.R + 360.0f, 360.0f);

				NewColor.G = FMath::Clamp(OffsetPos.Size() * 2.0f, 0.0f, 1.0f);
			}

			CurrentValueHSV = NewColor;
			
			OnColorChanged.Broadcast(NewColor);

		}));

	return MyXYSquare.ToSharedRef();
}

void UColorCircle::ReleaseSlateResources(bool bReleaseChildren) {
	Super::ReleaseSlateResources(bReleaseChildren);
	MyXYSquare.Reset();
}


void UColorCircle::EnsureMID() {
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

void UColorCircle::UpdateMID() {
	EnsureMID();
	if (BackgroundMID) {
		BackgroundMID->SetScalarParameterValue(TEXT("Hue"), CurrentValueHSV.R / 360.f);
		BackgroundMID->SetScalarParameterValue(TEXT("Saturation"), CurrentValueHSV.G);
		BackgroundMID->SetScalarParameterValue(TEXT("Value"), CurrentValueHSV.B);
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

FVector2D UColorCircle::ClampToCircle(FVector2D in) {
	const FVector2D half = FVector2D(.5, .5);

	float len = (in - half).Size();
	if (len <= .5f) return in;

	len = .5f;
	FVector2D n = (in - half).GetSafeNormal();
	return (n * len) + half;
}

void UColorCircle::SynchronizeProperties() {
	Super::SynchronizeProperties();
	UpdateMID();
}

void UColorCircle::SetXY(FVector2D newValue) {
	CurrentPos = ClampToCircle(newValue);

	if (MyXYSquare.IsValid()) {
		MyXYSquare->Invalidate(EInvalidateWidget::Paint);
	}
}

void UColorCircle::SetColor(FLinearColor NewColor) {
	FLinearColor colNew = NewColor.LinearRGBToHSV();
	if ((colNew.G == 0 || colNew.B == 0) && colNew.R == 0) {
		colNew.R = CurrentValueHSV.R;
	}
	SetColorHSV(colNew);
}

void UColorCircle::SetColorHSV(FLinearColor NewColorHSV) {
	
	//TODO Color calculation
	//TODO SetXY

	CurrentValueHSV = NewColorHSV;

	UpdateMID();
}

FLinearColor UColorCircle::GetColorHSV() {
	return CurrentValueHSV;
}

FLinearColor UColorCircle::GetColor() {
	return GetColorHSV().HSVToLinearRGB();
}

void UColorCircle::ApplyMatBrush(FSlateBrush& Brush, const FVector2D& Size, UMaterialInterface* Mat, FLinearColor Tint) {
	Brush.SetResourceObject(Mat);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = Size;
	Brush.TintColor = FSlateColor(Tint);
}
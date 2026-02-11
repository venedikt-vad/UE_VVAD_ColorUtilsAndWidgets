#include "ColorSquare.h"
#include "SColorSquare.h"

UColorSquare::UColorSquare(const FObjectInitializer& ObjectInitializer) {
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SV_MatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/HSV/M_UI_Gradient_Hue.M_UI_Gradient_Hue'"));
	if (SV_MatFinder.Succeeded() && SV_MatFinder.Object) {
		SV_Mat = SV_MatFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HS_MatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/HSV/M_UI_Gradient_Saturation.M_UI_Gradient_Saturation'"));
	if (HS_MatFinder.Succeeded() && HS_MatFinder.Object) {
		HS_Mat = HS_MatFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ThumbMatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/M_UI_Thumb.M_UI_Thumb'"));
	if (ThumbMatFinder.Succeeded() && ThumbMatFinder.Object) {
		UMaterialInterface* ThumbMat = ThumbMatFinder.Object;

		ApplyMatBrush(KnobBrush, FVector2D(10, 10), ThumbMat);
		ApplyMatBrush(KnobBrushHovered, FVector2D(10, 10), ThumbMat);
		ApplyMatBrush(KnobBrushDisabled, FVector2D(10, 10), ThumbMat);

		ApplyMatBrush(DarkKnobBrush, FVector2D(10, 10), ThumbMat, FLinearColor::Black);
		ApplyMatBrush(DarkKnobBrushHovered, FVector2D(10, 10), ThumbMat, FLinearColor::Black);
		ApplyMatBrush(DarkKnobBrushDisabled, FVector2D(10, 10), ThumbMat, FLinearColor::Black);
	}
}


TSharedRef<SWidget> UColorSquare::RebuildWidget() {
	MyXYSquare = SNew(SColorSquare)
		.XY(TAttribute<FVector2D>::Create(TAttribute<FVector2D>::FGetter::CreateUObject(this, &UColorSquare::GetXY)))
		.OnXYChanged(FOnXYChangedNative::CreateLambda([this](FVector2D XY) {
		CurrentPos = XY;
		OnXYChanged.Broadcast(CurrentPos);
			}));

	return MyXYSquare.ToSharedRef();
}

void UColorSquare::ReleaseSlateResources(bool bReleaseChildren) {
	Super::ReleaseSlateResources(bReleaseChildren);
	MyXYSquare.Reset();
}



void UColorSquare::EnsureMID() {
	if (!BackgroundMaterial) { BackgroundMID = nullptr; return; }

	if (!BackgroundMID || BackgroundMID->Parent != BackgroundMaterial) {
		BackgroundMID = UMaterialInstanceDynamic::Create(BackgroundMaterial, this);
		BackgroundBrush = FSlateBrush();
		BackgroundBrush.DrawAs = ESlateBrushDrawType::Image;
		BackgroundBrush.SetResourceObject(BackgroundMID);
		BackgroundBrush.ImageSize = FVector2D(256.f, 256.f);
	}
}

void UColorSquare::SynchronizeProperties() {
	Super::SynchronizeProperties();

	EnsureMID();

	if (BackgroundMID) {
		// Match your material param name + units
		//BackgroundMID->SetScalarParameterValue(TEXT("HueDegrees"), HueDegrees);
	}

	if (MyXYSquare.IsValid()) {
		MyXYSquare->SetBackgroundBrush(BackgroundMID ? &BackgroundBrush : nullptr);
		MyXYSquare->SetKnobBrush(&KnobBrush);
		//MyXYSquare->SetKnobSizeOverride(KnobSizeOverride);
	}
}



void UColorSquare::SetXY(FVector2D newValue) {
	CurrentPos.X = FMath::Clamp(newValue.X, 0.f, 1.f);
	CurrentPos.Y = FMath::Clamp(newValue.Y, 0.f, 1.f);

	if (MyXYSquare.IsValid()) {
		MyXYSquare->Invalidate(EInvalidateWidget::Paint);
	}

	//TODO: Update color values
}

void UColorSquare::SetColor(FLinearColor NewColor) {
	FLinearColor colNew = NewColor.LinearRGBToHSV();
	if ((colNew.G == 0 || colNew.B == 0) && colNew.R == 0) {
		colNew.R = CurrentValueHSV.R;
	}
	SetColorHSV(colNew);
}

void UColorSquare::SetColorHSV(FLinearColor NewColorHSV) {
	switch (SelectorType) {
	case EColorSelector2DType::SVSquare:
		SetXY(FVector2D(NewColorHSV.G, NewColorHSV.B));
		break;
	case EColorSelector2DType::HSSquare:
		SetXY(FVector2D(NewColorHSV.R/360, NewColorHSV.G));
		break;
	default:
		break;
	}
	CurrentValueHSV = NewColorHSV;

	EnsureMID();
	if (BackgroundMID) {
		//BackgroundMID->SetScalarParameterValue(TEXT("HueDegrees"), NewColorHSV.R);
	}
	OnColorChanged.Broadcast(NewColorHSV.HSVToLinearRGB());
}

FLinearColor UColorSquare::GetColorHSV() {
	return CurrentValueHSV;
}

FLinearColor UColorSquare::GetColor() {
	return GetColorHSV().HSVToLinearRGB();
}

void UColorSquare::ApplyMatBrush(FSlateBrush& Brush, const FVector2D& Size, UMaterialInterface* Mat, FLinearColor Tint) {
	Brush.SetResourceObject(Mat);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = Size;
	Brush.TintColor = FSlateColor(Tint);
}
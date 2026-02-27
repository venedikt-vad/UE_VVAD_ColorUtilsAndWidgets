#include "ColorSquare.h"
#include "SColorSquare.h"

UColorSquare::UColorSquare(const FObjectInitializer& ObjectInitializer) {
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SV_MatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/HSV/M_UI_SVSquare.M_UI_SVSquare'"));
	if (SV_MatFinder.Succeeded() && SV_MatFinder.Object) {
		SV_Mat = SV_MatFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HS_MatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/HSV/M_UI_HSSquare.M_UI_HSSquare'"));
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


TSharedRef<SWidget> UColorSquare::RebuildWidget() {
	MyXYSquare = SNew(SColorSquare)
		.XY(TAttribute<FVector2D>::Create(TAttribute<FVector2D>::FGetter::CreateUObject(this, &UColorSquare::GetXY)))
		.OnXYChanged(FOnXYChangedNative::CreateLambda([this](FVector2D XY) {
		
			CurrentPos = XY;
		
			OnXYChanged.Broadcast(CurrentPos);
		
			FLinearColor NewColor = CurrentValueHSV;
			switch (SelectorType) {
			case EColorSquareSelectorType::SVSquare:
				NewColor.G = FMath::Pow(CurrentPos.X, 0.5f);  //0.5 Power in material TODO: Make dynamic
				NewColor.B = FMath::Pow(CurrentPos.Y, 2.f);  //2.0 Power in material TODO: Make dynamic
				break;
			case EColorSquareSelectorType::HSSquare:
				NewColor.R = CurrentPos.X * 360.f; 
				NewColor.G = FMath::Pow(1 - CurrentPos.Y, .5f);  //0.5 Power in material TODO: Make dynamic
				break;
			default:
				break;
			}
			CurrentValueHSV = NewColor;
			
			OnColorChanged.Broadcast(NewColor);

		}));

	SetXY(FVector2D(.5f, .5f));

	return MyXYSquare.ToSharedRef();
}

void UColorSquare::ReleaseSlateResources(bool bReleaseChildren) {
	Super::ReleaseSlateResources(bReleaseChildren);
	MyXYSquare.Reset();
}



void UColorSquare::EnsureMID() {
	switch (SelectorType) {
	case EColorSquareSelectorType::SVSquare:
		BackgroundMaterial = SV_Mat;
		break;
	case EColorSquareSelectorType::HSSquare:
		BackgroundMaterial = HS_Mat;
		break;
	default:
		break;
	}

	if (!BackgroundMaterial) { BackgroundMID = nullptr; return; }

	if (!BackgroundMID || BackgroundMID->Parent != BackgroundMaterial) {
		BackgroundMID = UMaterialInstanceDynamic::Create(BackgroundMaterial, this);
		BackgroundBrush = FSlateBrush();
		BackgroundBrush.DrawAs = ESlateBrushDrawType::Image;
		BackgroundBrush.SetResourceObject(BackgroundMID);
		BackgroundBrush.ImageSize = FVector2D(256.f, 256.f);
	}
}

void UColorSquare::UpdateMID() {
	EnsureMID();
	if (BackgroundMID) {
		BackgroundMID->SetScalarParameterValue(TEXT("Hue"), CurrentValueHSV.R / 360.f);
		BackgroundMID->SetScalarParameterValue(TEXT("Saturation"), CurrentValueHSV.G);
		BackgroundMID->SetScalarParameterValue(TEXT("Value"), CurrentValueHSV.B);
	}
	if (MyXYSquare.IsValid()) {
		MyXYSquare->SetBackgroundBrush(BackgroundMID ? &BackgroundBrush : nullptr);
		if (CurrentValueHSV.B > 0.5 && bUseDarkKnobOnLightSurface) {
			MyXYSquare->SetKnobBrush(&DarkKnobBrush);
		} else {
			MyXYSquare->SetKnobBrush(&KnobBrush);

		}
	}
}

void UColorSquare::SynchronizeProperties() {
	Super::SynchronizeProperties();
	UpdateMID();
}

void UColorSquare::SetXY(FVector2D newValue) {
	CurrentPos.X = FMath::Clamp(newValue.X, 0.f, 1.f);
	CurrentPos.Y = FMath::Clamp(newValue.Y, 0.f, 1.f);

	if (MyXYSquare.IsValid()) {
		MyXYSquare->Invalidate(EInvalidateWidget::Paint);
	}

}

void UColorSquare::SetColor(FLinearColor NewColor) {
	FLinearColor colNew = NewColor.LinearRGBToHSV();
	if ((colNew.G == 0 || colNew.B == 0) && colNew.R == 0) {
		colNew.R = CurrentValueHSV.R;
	}
	SetColorHSV(colNew);
}

void UColorSquare::SetColorHSV(FLinearColor NewColorHSV) {
	CurrentValueHSV = NewColorHSV;

	switch (SelectorType) {
	case EColorSquareSelectorType::SVSquare:
		NewColorHSV.G = FMath::Pow(NewColorHSV.G, 1.f / 0.5f);
		NewColorHSV.B = FMath::Pow(NewColorHSV.B, 1.f / 2.f);
		SetXY(FVector2D(NewColorHSV.G, NewColorHSV.B));
		break;
	case EColorSquareSelectorType::HSSquare:
		SetXY(FVector2D(NewColorHSV.R/360, 1 - FMath::Pow(NewColorHSV.G, 1.f / .5f)));
		break;
	default:
		break;
	}

	UpdateMID();
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

#if WITH_EDITOR
const FText UColorSquare::GetPaletteCategory() {
	return NSLOCTEXT("VVAD_COLORUTILSANDWIDGETS", "Color Ultility Widgets", "Color");
}
#endif
// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorGradientSlider.h"

UColorGradientSlider::UColorGradientSlider(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	//Load from plugin dir:  
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HueMatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/HSV/M_UI_Gradient_Hue.M_UI_Gradient_Hue'"));
	if (HueMatFinder.Succeeded() && HueMatFinder.Object) {
		HueMat = HueMatFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SatMatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/HSV/M_UI_Gradient_Saturation.M_UI_Gradient_Saturation'"));
	if (SatMatFinder.Succeeded() && SatMatFinder.Object) {
		SatMat = SatMatFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ValMatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/HSV/M_UI_Gradient_Value.M_UI_Gradient_Value'"));
	if (ValMatFinder.Succeeded() && ValMatFinder.Object) {
		ValMat = ValMatFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> RedMatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/RGB/M_UI_Gradient_R.M_UI_Gradient_R'"));
	if (RedMatFinder.Succeeded() && RedMatFinder.Object) {
		RedMat = RedMatFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> GreenMatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/RGB/M_UI_Gradient_G.M_UI_Gradient_G'"));
	if (GreenMatFinder.Succeeded() && GreenMatFinder.Object) {
		GreenMat = GreenMatFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BlueMatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/RGB/M_UI_Gradient_B.M_UI_Gradient_B'"));
	if (BlueMatFinder.Succeeded() && BlueMatFinder.Object) {
		BlueMat = BlueMatFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ThumbMatFinder(
		TEXT("Material'/VVAD_ColorUtilsAndWidgets/Materials/M_UI_Thumb.M_UI_Thumb'"));
	if (ThumbMatFinder.Succeeded() && ThumbMatFinder.Object) {
		UMaterialInterface* ThumbMat = ThumbMatFinder.Object;

		ApplyMatBrush(NormalThumb, FVector2D(8, 60), ThumbMat);
		ApplyMatBrush(HoveredThumb, FVector2D(8, 60), ThumbMat);
		ApplyMatBrush(DisabledThumb, FVector2D(8, 60), ThumbMat);
	}

}

FLinearColor UColorGradientSlider::GetColorValue() const {
	return CurrentValue;
}

void UColorGradientSlider::SetColorValue(const FLinearColor& NewValue) {
	CurrentValue = NewValue;
	switch (SliderType) {
	case EColorSliderType::Hue:
		Value = NewValue.LinearRGBToHSV().R / 360;
		break;
	case EColorSliderType::Saturation:
		Value = NewValue.LinearRGBToHSV().G;
		break;
	case EColorSliderType::Value:
		Value = NewValue.LinearRGBToHSV().B;
		break;
	case EColorSliderType::Red:
		Value = NewValue.R;
		break;
	case EColorSliderType::Green:
		Value = NewValue.G;
		break;
	case EColorSliderType::Blue:
		Value = NewValue.B;
		break;
	case EColorSliderType::Custom:
		break;
	default:
		break;
	}

}

void UColorGradientSlider::SynchronizeProperties() {
	UpdateWidgetDetails();
	Super::SynchronizeProperties();
}

#if WITH_EDITOR
void UColorGradientSlider::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (HasAnyFlags(RF_ClassDefaultObject)) return;
	UpdateWidgetDetails();
	if (GetCachedWidget().IsValid()) {
		Super::SynchronizeProperties();
		InvalidateLayoutAndVolatility();
	}
}
#endif

void UColorGradientSlider::UpdateWidgetDetails() {
	WidgetStyle.BarThickness = Thickness;

	const FVector2D BarSize(8.f, 8.f);

	UMaterialInterface* mat = nullptr;

	switch (SliderType) {
	case EColorSliderType::Hue:
		mat = HueMat;
		break;
	case EColorSliderType::Saturation:
		mat = SatMat;
		break;
	case EColorSliderType::Value:
		mat = ValMat;
		break;
	case EColorSliderType::Red:
		mat = RedMat;
		break;
	case EColorSliderType::Green:
		mat = GreenMat;
		break;
	case EColorSliderType::Blue:
		mat = BlueMat;
		break;
	case EColorSliderType::Custom:
		mat = nullptr; {
			//apply gradient image
		}
		break;
	default:
		mat = HueMat;
		break;
	}

	if (mat) {
		ApplyMatBrush(WidgetStyle.NormalBarImage, BarSize, mat);
		ApplyMatBrush(WidgetStyle.HoveredBarImage, BarSize, mat);
		ApplyMatBrush(WidgetStyle.DisabledBarImage, BarSize, mat);
	}
	
	if (bFixateThumbToThicknass) {
		NormalThumb.SetImageSize(FVector2D(NormalThumb.GetImageSize().X, Thickness + ThumbThicknessOffset));
		HoveredThumb.SetImageSize(FVector2D(HoveredThumb.GetImageSize().X, Thickness + ThumbThicknessOffset));
		DisabledThumb.SetImageSize(FVector2D(DisabledThumb.GetImageSize().X, Thickness + ThumbThicknessOffset));
	}
	WidgetStyle.NormalThumbImage = NormalThumb;
	WidgetStyle.HoveredThumbImage = HoveredThumb;
	WidgetStyle.DisabledThumbImage = DisabledThumb;
}

void UColorGradientSlider::ApplyMatBrush(FSlateBrush& Brush, const FVector2D& Size, UMaterialInterface* Mat) {
	Brush.SetResourceObject(Mat);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = Size;
	Brush.TintColor = FSlateColor(FLinearColor::White);
}

#if WITH_EDITOR
const FText UColorGradientSlider::GetPaletteCategory() {
	return NSLOCTEXT("VVAD_COLORUTILSANDWIDGETS", "Color Ultility Widgets", "Color");
}
#endif

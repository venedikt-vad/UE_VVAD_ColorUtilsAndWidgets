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

	OnValueChanged.AddUniqueDynamic(this, &UColorGradientSlider::HandleSliderValueChanged);
}

FLinearColor UColorGradientSlider::GetColorValue() const {
	return CurrentValueHSV.HSVToLinearRGB();
}

void UColorGradientSlider::SetColorValue(const FLinearColor& NewValue) {
	switch (SliderType) {
	case EColorSliderType::Hue:
		if (NewValue.LinearRGBToHSV().G == 0 || NewValue.LinearRGBToHSV().B == 0) break;
		Super::SetValue(NewValue.LinearRGBToHSV().R / 360);
		break;
	case EColorSliderType::Saturation:
		if (NewValue.LinearRGBToHSV().B == 0) break;
		Super::SetValue(NewValue.LinearRGBToHSV().G);
		break;
	case EColorSliderType::Value:
		if (NewValue.LinearRGBToHSV().G == 0) break;
		Super::SetValue(NewValue.LinearRGBToHSV().B);
		break;
	case EColorSliderType::Red:
		Super::SetValue(NewValue.R);
		break;
	case EColorSliderType::Green:
		Super::SetValue(NewValue.G);
		break;
	case EColorSliderType::Blue:
		Super::SetValue(NewValue.B);
		break;
	case EColorSliderType::Custom:
		break;
	default:
		break;
	}

	FLinearColor v = NewValue.LinearRGBToHSV();
	if (v.LinearRGBToHSV().G != 0 && v.LinearRGBToHSV().B != 0) {
		CurrentValueHSV.R = v.R;
	}
	CurrentValueHSV.G = v.G;
	CurrentValueHSV.B = v.B;


	UpdateMatInst();
}

FLinearColor UColorGradientSlider::GetColorValueHSV() const {
	return CurrentValueHSV;
}

void UColorGradientSlider::SetColorValueHSV(const FLinearColor& NewValueHSV) {
	switch (SliderType) {
	case EColorSliderType::Hue:
		Super::SetValue(NewValueHSV.R / 360);
		break;
	case EColorSliderType::Saturation:
		Super::SetValue(NewValueHSV.G);
		break;
	case EColorSliderType::Value:
		Super::SetValue(NewValueHSV.B);
		break;
	case EColorSliderType::Red:
		Super::SetValue(NewValueHSV.HSVToLinearRGB().R);
		break;
	case EColorSliderType::Green:
		Super::SetValue(NewValueHSV.HSVToLinearRGB().G);
		break;
	case EColorSliderType::Blue:
		Super::SetValue(NewValueHSV.HSVToLinearRGB().B);
		break;
	case EColorSliderType::Custom:
		break;
	default:
		break;
	}

	CurrentValueHSV = NewValueHSV;

	UpdateMatInst();
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

void UColorGradientSlider::HandleSliderValueChanged(float InValue) {
	FLinearColor NewValue = CurrentValueHSV;
	switch (SliderType) {
	case EColorSliderType::Hue:
		NewValue.R = GetValue() * 360;
		break;
	case EColorSliderType::Saturation:
		NewValue.G = GetValue();
		break;
	case EColorSliderType::Value:
		NewValue.B = GetValue();
		break;
	case EColorSliderType::Red:
		NewValue = NewValue.HSVToLinearRGB();
		NewValue.R = GetValue();
		NewValue = NewValue.LinearRGBToHSV();

		break;
	case EColorSliderType::Green:
		NewValue = NewValue.HSVToLinearRGB();
		NewValue.G = GetValue();
		NewValue = NewValue.LinearRGBToHSV();
		break;
	case EColorSliderType::Blue:
		NewValue = NewValue.HSVToLinearRGB();
		NewValue.B = GetValue();
		NewValue = NewValue.LinearRGBToHSV();
		break;
	case EColorSliderType::Custom:
		break;
	default:
		break;
	}
	CurrentValueHSV = NewValue;
}


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

	MatInst = UMaterialInstanceDynamic::Create(mat, this);

	if (mat) {
		ApplyMatBrush(WidgetStyle.NormalBarImage, BarSize, MatInst);
		ApplyMatBrush(WidgetStyle.HoveredBarImage, BarSize, MatInst);
		ApplyMatBrush(WidgetStyle.DisabledBarImage, BarSize, MatInst);
	}
	UpdateMatInst();
	
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

void UColorGradientSlider::UpdateMatInst() {
	MatInst->SetScalarParameterValue(TEXT("Hue"), CurrentValueHSV.R / 360.f);
	MatInst->SetScalarParameterValue(TEXT("Saturation"), CurrentValueHSV.G);
	MatInst->SetScalarParameterValue(TEXT("Value"), CurrentValueHSV.B);

	MatInst->SetScalarParameterValue(TEXT("R"), CurrentValueHSV.HSVToLinearRGB().R);
	MatInst->SetScalarParameterValue(TEXT("G"), CurrentValueHSV.HSVToLinearRGB().G);
	MatInst->SetScalarParameterValue(TEXT("B"), CurrentValueHSV.HSVToLinearRGB().B);
}

#if WITH_EDITOR
const FText UColorGradientSlider::GetPaletteCategory() {
	return NSLOCTEXT("VVAD_COLORUTILSANDWIDGETS", "Color Ultility Widgets", "Color");
}
#endif

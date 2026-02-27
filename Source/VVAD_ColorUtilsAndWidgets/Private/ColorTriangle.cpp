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
		ApplyMatBrush(RingKnobBrush, FVector2D(10, 10), ThumbMat);
		//ApplyMatBrush(KnobBrushHovered, FVector2D(10, 10), ThumbMat);
		//ApplyMatBrush(KnobBrushDisabled, FVector2D(10, 10), ThumbMat);

		ApplyMatBrush(DarkKnobBrush, FVector2D(10, 10), ThumbMat, FLinearColor::Black);
		ApplyMatBrush(DarkRingKnobBrush, FVector2D(10, 10), ThumbMat, FLinearColor::Black);
		//ApplyMatBrush(DarkKnobBrushHovered, FVector2D(10, 10), ThumbMat, FLinearColor::Black);
		//ApplyMatBrush(DarkKnobBrushDisabled, FVector2D(10, 10), ThumbMat, FLinearColor::Black);
	}
}


TSharedRef<SWidget> UColorTriangle::RebuildWidget() {
	MyXYSquare = SNew(SColorTriangle)
		.XY(TAttribute<FVector2D>::Create(TAttribute<FVector2D>::FGetter::CreateUObject(this, &UColorTriangle::GetXY)))
		.RingValue(TAttribute<float>::Create(TAttribute<float>::FGetter::CreateUObject(this, &UColorTriangle::GetRingAngle)))
		.OnXYChanged(FOnTriangleXYChangedNative::CreateLambda([this](FVector2D XY) {
		
			//CurrentPos = ClampToCircle(XY);
			CurrentPos = XY;

			OnXYChanged.Broadcast(CurrentPos); //TODO Remove
		
			FLinearColor NewColor = CurrentValueHSV;
			
			{
				FVector2D uvec = FVector2D(0, InnerRadius * .5f).GetRotated(-90 - CurrentValueHSV.R - HueOffset);
				FVector c = Barycentric(CurrentPos*.5f, uvec.GetRotated(-120), uvec, uvec.GetRotated(120));
				NewColor.G = FMath::Clamp(((c.Z + c.X) > 1e-6f) ? (c.X / (c.Z + c.X)) : 0.0f, 0.f,1.f);
				NewColor.B = pow(1.f - FMath::Clamp(c.Y, 0.f, 1.f), 3.f);
			}

			CurrentValueHSV = NewColor;
			
			OnColorChanged.Broadcast(NewColor);

		}))
		.OnRingChanged(FOnRingChangedNative::CreateLambda([this](float Angle){
			FLinearColor NewColor = CurrentValueHSV;
			NewColor.R = FMath::Fmod(Angle + 360.0f, 360.0f);
			CurrentValueHSV = NewColor;
			OnColorChanged.Broadcast(NewColor);
		}));

	SetXY(FVector2D(.5f, .5f));

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
		//TODO bRotateWithHue 
		BackgroundMID->SetScalarParameterValue(TEXT("Hue"), CurrentValueHSV.R / 360.f);
		BackgroundMID->SetScalarParameterValue(TEXT("Radius"), InnerRadius);
		BackgroundMID->SetScalarParameterValue(TEXT("HueOffset"), -HueOffset / 360.f);
	}
	if (MyXYSquare.IsValid()) {
		MyXYSquare->SetRotateRingKnob(true); //TODO: Add user parameter
		MyXYSquare->SetRadius(InnerRadius);
		MyXYSquare->SetBackgroundBrush(BackgroundMID ? &BackgroundBrush : nullptr);
		if (CurrentValueHSV.B > 0.5 && bUseDarkKnobOnLightSurface) {
			MyXYSquare->SetKnobBrush(&DarkKnobBrush);
			MyXYSquare->SetRingKnobBrush(&DarkRingKnobBrush);
		} else {
			MyXYSquare->SetKnobBrush(&KnobBrush);
			MyXYSquare->SetRingKnobBrush(&RingKnobBrush);
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

void UColorTriangle::SetRingAngle(float AngleValue) {
	CurrentAngle = FMath::Max(0.f, FMath::Fmod(AngleValue, 360.0f));
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

	//TODO Udate to triangle

	const FVector2D uvec = FVector2D(0, InnerRadius * .5f).GetRotated(-90 -NewColorHSV.R - HueOffset);

	FVector2D target = PointFromSV(NewColorHSV.G, pow(NewColorHSV.B, 1./3.), uvec.GetRotated(-120), uvec, uvec.GetRotated(120));
	target += FVector2D(.5f, .5f);


	CurrentAngle = CurrentValueHSV.R;

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


FVector UColorTriangle::Barycentric(FVector2D P, FVector2D W, FVector2D H, FVector2D B){
	FVector2D v0{ H.X - W.X, H.Y - W.Y };
	FVector2D v1{ B.X - W.X, B.Y - W.Y };
	FVector2D v2{ P.X - W.X, P.Y - W.Y };

	float den = cross(v0, v1);
	float a = cross(v2, v1) / den;
	float b = cross(v0, v2) / den;
	float c = 1.0f - a - b;

	float wH = a;
	float wB = b;
	float wW = c;

	return FVector(wH, wB, wW);
}

FVector2D UColorTriangle::PointFromSV(
	float S, float V,
	FVector2D W, FVector2D H, FVector2D B){
	S = FMath::Clamp(S, 0.0f, 1.0f);
	V = FMath::Clamp(V, 0.0f, 1.0f);

	float wB = 1.0f - V;
	float wH = S * V;
	float wW = (1.0f - S) * V;

	FVector2D P;
	P.X = wW * W.X + wH * H.X + wB * B.X;
	P.Y = wW * W.Y + wH * H.Y + wB * B.Y;

	return P;
}

#if WITH_EDITOR
const FText UColorTriangle::GetPaletteCategory() {
	return NSLOCTEXT("VVAD_COLORUTILSANDWIDGETS", "Color Ultility Widgets", "Color");
}
#endif
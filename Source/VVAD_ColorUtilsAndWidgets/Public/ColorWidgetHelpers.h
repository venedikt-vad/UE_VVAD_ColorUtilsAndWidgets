#pragma once
#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"

static void ApplyMatBrush(FSlateBrush& Brush, const FVector2D& Size, UMaterialInterface* Mat, FLinearColor Tint = FLinearColor::White) {
	Brush.SetResourceObject(Mat);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = Size;
	Brush.TintColor = FSlateColor(Tint);
};

static FVector2D ClampToCircle(FVector2D in) {
	const FVector2D half = FVector2D(.5, .5);

	float len = (in - half).Size();
	if (len <= .5f) return in;

	len = .5f;
	FVector2D n = (in - half).GetSafeNormal();
	return (n * len) + half;
}
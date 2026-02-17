#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Styling/SlateBrush.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ColorCircle.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCircleXYChangedBP, FVector2D, XY);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCircleColorChangedBP, FLinearColor, color);

UCLASS(BlueprintType, Blueprintable, meta=(DisplayName="Color Circle"))
class VVAD_COLORUTILSANDWIDGETS_API UColorCircle : public UWidget {
	GENERATED_BODY()
public:
	UColorCircle(const FObjectInitializer& ObjectInitializer);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance|Knob|Light")
	FSlateBrush KnobBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob|Dark")
	bool bUseDarkKnobOnLightSurface = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob|Dark")
	FSlateBrush DarkKnobBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance|Knob")
	FVector2D KnobSizeOverride = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	FVector2D CurrentPos = FVector2D::ZeroVector;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCircleXYChangedBP OnXYChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCircleColorChangedBP OnColorChanged;

	UFUNCTION(BlueprintCallable) void SetColor(FLinearColor NewColor);
	UFUNCTION(BlueprintCallable) void SetColorHSV(FLinearColor NewColorHSV);
	
	UFUNCTION(BlueprintCallable) FLinearColor GetColor();
	UFUNCTION(BlueprintCallable) FLinearColor GetColorHSV();

	UFUNCTION(BlueprintCallable) void SetXY(FVector2D newValue);
	UFUNCTION(BlueprintCallable) FVector2D GetXY() const { return CurrentPos; }

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* BackgroundMID = nullptr;
	FSlateBrush BackgroundBrush;
	TSharedPtr<class SColorSquare> MyXYSquare;

	void EnsureMID();
	void UpdateMID();

	//TODO Move to WidgetHelperFunctions.h
	void ApplyMatBrush(FSlateBrush& Brush, const FVector2D& Size, UMaterialInterface* Mat, FLinearColor Tint = FLinearColor::White);

	FVector2D ClampToCircle(FVector2D in);

	UPROPERTY(Transient)
	FLinearColor CurrentValueHSV = FLinearColor(1, 0, 0, 1);

	UPROPERTY(Transient)
	UMaterialInterface* SV_Mat = nullptr;
	UPROPERTY(Transient)
	UMaterialInterface* HS_Mat = nullptr;

	UPROPERTY(Transient)
	UMaterialInterface* BackgroundMaterial = nullptr;
};

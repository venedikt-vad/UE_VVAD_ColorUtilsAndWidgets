#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Styling/SlateBrush.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ColorTriangle.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriangleColorChangedBP, FLinearColor, color);

UCLASS(BlueprintType, Blueprintable, meta=(DisplayName="Color Triangle"))
class VVAD_COLORUTILSANDWIDGETS_API UColorTriangle : public UWidget {
	GENERATED_BODY()
public:
	UColorTriangle(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (ClampMin = "0.0", ClampMax = "360.0", UIMin = "0.0", UIMax = "360.0", Units = "Degrees"))
	float HueOffset = 270;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", Units = "Percent"))
	float InnerRadius = .8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	bool bRotateWithHue = true;  //TODO Fix offset when false

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob")
	bool bRotateRingKnob = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance|Knob|Light")
	FSlateBrush KnobBrush;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob|Light")
	FSlateBrush RingKnobBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob|Dark")
	bool bUseDarkKnobOnLightSurface = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob|Dark")
	FSlateBrush DarkKnobBrush;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob|Dark")
	FSlateBrush DarkRingKnobBrush;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance|Knob")
	//FVector2D KnobSize = FVector2D(10,10);
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob")
	//FVector2D RingKnobSize = FVector2D(100, 10);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	FVector2D CurrentPos = FVector2D::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	float CurrentAngle = 0;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTriangleColorChangedBP OnColorChanged;

	UFUNCTION(BlueprintCallable, Category = "ColorValue") void SetColor(FLinearColor NewColor);
	UFUNCTION(BlueprintCallable, Category = "ColorValue") void SetColorHSV(FLinearColor NewColorHSV);
	
	UFUNCTION(BlueprintCallable, Category = "ColorValue") FLinearColor GetColor();
	UFUNCTION(BlueprintCallable, Category = "ColorValue") FLinearColor GetColorHSV();

	UFUNCTION(BlueprintCallable, Category = "ColorValue|RawValue") void SetXY(FVector2D newValue);
	UFUNCTION(BlueprintCallable, Category = "ColorValue|RawValue") void SetRingAngle(float AngleValue);

	UFUNCTION(BlueprintCallable, Category = "ColorValue|RawValue") FVector2D GetXY() const { return CurrentPos; }
	UFUNCTION(BlueprintCallable, Category = "ColorValue|RawValue") float GetRingAngle() const { return CurrentAngle; }

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* BackgroundMID = nullptr;
	FSlateBrush BackgroundBrush;
	TSharedPtr<class SColorTriangle> MyXYSquare;

	void EnsureMID();
	void UpdateMID();
	void KnobColorUpdate();

	static float cross(FVector2D a, FVector2D b) { return a.X * b.Y - a.Y * b.X; };
	FVector2D PointFromSV(float S, float V, FVector2D W, FVector2D H, FVector2D B);
	FVector Barycentric(FVector2D P, FVector2D W, FVector2D H, FVector2D B);

	UPROPERTY(Transient)
	FLinearColor CurrentValueHSV = FLinearColor(0, 1, 1, 1);

	UPROPERTY(Transient)
	UMaterialInterface* SV_Mat = nullptr;
	UPROPERTY(Transient)
	UMaterialInterface* HS_Mat = nullptr;

	UPROPERTY(Transient)
	UMaterialInterface* BackgroundMaterial = nullptr;
};

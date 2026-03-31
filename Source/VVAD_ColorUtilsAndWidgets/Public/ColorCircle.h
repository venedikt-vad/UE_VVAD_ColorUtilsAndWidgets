#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Styling/SlateBrush.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ColorCircle.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCircleColorChangedBP, FLinearColor, color);

UCLASS(BlueprintType, Blueprintable, meta=(DisplayName="Color Circle"))
class VVAD_COLORUTILSANDWIDGETS_API UColorCircle : public UWidget {
	GENERATED_BODY()
public:
	UColorCircle(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (ClampMin = "0.0", ClampMax = "360.0", UIMin = "0.0", UIMax = "360.0", Units = "Degrees"))
	float HueOffset = 270;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance|Knob|Light")
	FSlateBrush KnobBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob|Dark")
	bool bUseDarkKnobOnLightSurface = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob|Dark")
	FSlateBrush DarkKnobBrush;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance|Knob")
	FVector2D KnobSizeOverride = FVector2D::ZeroVector;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	FVector2D CurrentPos = FVector2D::ZeroVector;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCircleColorChangedBP OnColorChanged;

	UFUNCTION(BlueprintCallable, Category = "ColorValue") void SetColor(FLinearColor NewColor);
	UFUNCTION(BlueprintCallable, Category = "ColorValue") void SetColorHSV(FLinearColor NewColorHSV);
	
	UFUNCTION(BlueprintCallable, Category = "ColorValue") FLinearColor GetColor();
	UFUNCTION(BlueprintCallable, Category = "ColorValue") FLinearColor GetColorHSV();

	UFUNCTION(BlueprintCallable, Category = "ColorValue|RawValue") void SetXY(FVector2D newValue);
	UFUNCTION(BlueprintCallable, Category = "ColorValue|RawValue") FVector2D GetXY() const { return CurrentPos; }

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

	void KnobColorUpdate();

	UPROPERTY(Transient)
	FLinearColor CurrentValueHSV = FLinearColor(0, 1, 1, 1);

	UPROPERTY(Transient)
	UMaterialInterface* SV_Mat = nullptr;
	UPROPERTY(Transient)
	UMaterialInterface* HS_Mat = nullptr;

	UPROPERTY(Transient)
	UMaterialInterface* BackgroundMaterial = nullptr;
};

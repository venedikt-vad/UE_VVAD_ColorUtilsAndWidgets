#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Styling/SlateBrush.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ColorSquare.generated.h"

UENUM(Blueprintable)
enum class EColorSquareSelectorType : uint8 {
	SVSquare,
	HSSquare
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnXYChangedBP, FVector2D, XY);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorChangedBP, FLinearColor, color);

UCLASS(BlueprintType, Blueprintable, meta=(DisplayName="Color Square"))
class VVAD_COLORUTILSANDWIDGETS_API UColorSquare : public UWidget {
	GENERATED_BODY()
public:
	UColorSquare(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	EColorSquareSelectorType SelectorType = EColorSquareSelectorType::SVSquare;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance|Knob|Light")
	FSlateBrush KnobBrush;
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob|Light")
	FSlateBrush KnobBrushHovered;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob|Light")
	FSlateBrush KnobBrushDisabled;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob|Dark")
	bool bUseDarkKnobOnLightSurface = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob|Dark")
	FSlateBrush DarkKnobBrush;
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob|Dark")
	FSlateBrush DarkKnobBrushHovered;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Knob|Dark")
	FSlateBrush DarkKnobBrushDisabled;*/


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance|Knob")
	FVector2D KnobSizeOverride = FVector2D::ZeroVector;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	FVector2D CurrentPos = FVector2D::ZeroVector;


	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnXYChangedBP OnXYChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnColorChangedBP OnColorChanged;

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

	void ApplyMatBrush(FSlateBrush& Brush, const FVector2D& Size, UMaterialInterface* Mat, FLinearColor Tint = FLinearColor::White);

	UPROPERTY(Transient)
	FLinearColor CurrentValueHSV = FLinearColor(0, 1, 1, 1);

	UPROPERTY(Transient)
	UMaterialInterface* BackgroundMaterial = nullptr;
	UPROPERTY(Transient)
	UMaterialInterface* SV_Mat = nullptr;
	UPROPERTY(Transient)
	UMaterialInterface* HS_Mat = nullptr;
};

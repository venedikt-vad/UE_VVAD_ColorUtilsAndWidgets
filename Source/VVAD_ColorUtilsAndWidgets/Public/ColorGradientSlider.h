// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Slider.h"
#include "ColorGradientSlider.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EColorSliderType : uint8
{
	Hue        UMETA(DisplayName = "Hue"),
	Saturation UMETA(DisplayName = "Saturation"),
	Value      UMETA(DisplayName = "Value"),
	Red        UMETA(DisplayName = "R"),
	Green      UMETA(DisplayName = "G"),
	Blue       UMETA(DisplayName = "B"),
	Custom     UMETA(DisplayName = "Custom"),
};


UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Color Gradient Slider"), HideCategories = (Style))
class VVAD_COLORUTILSANDWIDGETS_API UColorGradientSlider : public USlider{
	GENERATED_BODY()
	
public:
	UColorGradientSlider(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	EColorSliderType SliderType = EColorSliderType::Hue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float Thickness = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Thumb")
	bool bFixateThumbToThicknass = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Thumb", meta = (EditCondition = "bFixateThumbToThicknass", HideEditConditionToggle))
	float ThumbThicknessOffset = 6.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Thumb")
	FSlateBrush NormalThumb;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Thumb")
	FSlateBrush HoveredThumb;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Thumb")
	FSlateBrush DisabledThumb;

	UFUNCTION(BlueprintCallable)
	FLinearColor GetColorValue() const;
	UFUNCTION(BlueprintCallable)
	void SetColorValue(const FLinearColor& NewValue);

	UFUNCTION(BlueprintCallable)
	FLinearColor GetColorValueHSV() const;
	UFUNCTION(BlueprintCallable)
	void SetColorValueHSV(const FLinearColor& NewValueHSV);

	//virtual override function for value update
	
	//Blueprint event for update of Color FColor ValueUpdated(); //Should be called each time know is tweaked
	

protected:
	virtual void SynchronizeProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION()
	void HandleSliderValueChanged(float InValue);

private:
	UPROPERTY(Transient)
	UMaterialInterface* HueMat = nullptr;
	UPROPERTY(Transient)
	UMaterialInterface* SatMat = nullptr;
	UPROPERTY(Transient)
	UMaterialInterface* ValMat = nullptr;
	UPROPERTY(Transient)
	UMaterialInterface* RedMat = nullptr;
	UPROPERTY(Transient)
	UMaterialInterface* GreenMat = nullptr;
	UPROPERTY(Transient)
	UMaterialInterface* BlueMat = nullptr;

	UPROPERTY(Transient)
	FLinearColor CurrentValueHSV = FLinearColor(1,0,0,1);
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* MatInst = nullptr;

	void UpdateWidgetDetails();
	void ApplyMatBrush(FSlateBrush& Brush, const FVector2D& Size, UMaterialInterface* Mat);

	void UpdateMatInst();
};

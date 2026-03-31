// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/Texture2D.h"
#include "Curves/CurveLinearColor.h"

#include "VVAD_ColorUtilsAndWidgetsBPLibrary.generated.h"
/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/

//Color space structs
USTRUCT(BlueprintType)
struct FColorCIELUV {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category = "Color|CIELUV")
	float L;
	UPROPERTY(BlueprintReadWrite, Category = "Color|CIELUV")
	float u;
	UPROPERTY(BlueprintReadWrite, Category = "Color|CIELUV")
	float v;

	FColorCIELUV() = default;
	FColorCIELUV(float L, float u, float v) :L(L), u(u), v(v) {};
	FColorCIELUV(FVector value) :L(value.X), u(value.Y), v(value.Z) {};
};

USTRUCT(BlueprintType)
struct FColorXYZ {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category = "Color|XYZ")
	float X;
	UPROPERTY(BlueprintReadWrite, Category = "Color|XYZ")
	float Y;
	UPROPERTY(BlueprintReadWrite, Category = "Color|XYZ")
	float Z;

	FColorXYZ() = default;
	FColorXYZ(float X, float Y, float Z) :X(X), Y(Y), Z(Z) {};
	FColorXYZ(FVector value) :X(value.X), Y(value.Y), Z(value.Z) {};
};

UCLASS()
class UVVAD_ColorUtilsAndWidgetsBPLibrary : public UBlueprintFunctionLibrary {
	GENERATED_UCLASS_BODY()


	//Color Space Convertations
	UFUNCTION(BlueprintCallable, meta = (Category = "Color|Convertations"))
	static FColorXYZ    sRGBToXYZcolor(FColor color);
	UFUNCTION(BlueprintCallable, meta = (Category = "Color|Convertations"))
	static FColorCIELUV sRGBToCIELUV(FColor color) {
		return XYZcolorToCIELUV(sRGBToXYZcolor(color));
	}
	UFUNCTION(BlueprintCallable, meta = (Category = "Color|Convertations"))
	static FColor       XYZcolorTosRGB(FColorXYZ XYZcolor);
	UFUNCTION(BlueprintCallable, meta = (Category = "Color|Convertations"))
	static FColorCIELUV XYZcolorToCIELUV(FColorXYZ XYZcolor);
	UFUNCTION(BlueprintCallable, meta = (Category = "Color|Convertations"))
	static FColorXYZ    CIELUVToXYZcolor(FColorCIELUV CIELUV);
	UFUNCTION(BlueprintCallable, meta = (Category = "Color|Convertations"))
	static FColor		CIELUVTosRGB(FColorCIELUV CIELUV) {
		return XYZcolorTosRGB(CIELUVToXYZcolor(CIELUV));
	}

	//CurveLinearColor
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", Category = "Color|Curve"))
	static UTexture2D* CurveLinearColor_CreateTexture(UObject* WorldContextObject, int32 Width, UCurveLinearColor* Curve, bool bOutputSRGB = true);
};

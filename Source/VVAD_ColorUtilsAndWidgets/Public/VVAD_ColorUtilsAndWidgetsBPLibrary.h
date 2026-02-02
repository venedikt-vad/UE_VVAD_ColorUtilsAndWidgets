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

UENUM()
enum FPalleteFileType {
	JASC,
	GIMP,
	PaintNET,
	ASEF,
	HEX,
};

UENUM(BlueprintType)
enum EColorSearchType {
	ClosestOffset = 4,
	ClosestLine = 3,
	ClosestX = 0,
	ClosestY = 1,
	ClosestZ = 2,
};

UENUM(BlueprintType)
enum EColorSpace {
	RGB,
	HSV,
	XYZ,
	CIELUV,
};

//Color space structs
USTRUCT(BlueprintType)
struct FColorCIELUV {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	float L;
	UPROPERTY(BlueprintReadWrite)
	float u;
	UPROPERTY(BlueprintReadWrite)
	float v;

	FColorCIELUV() = default;
	FColorCIELUV(float L, float u, float v) :L(L), u(u), v(v) {};
	FColorCIELUV(FVector value) :L(value.X), u(value.Y), v(value.Z) {};
};

USTRUCT(BlueprintType)
struct FColorXYZ {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	float X;
	UPROPERTY(BlueprintReadWrite)
	float Y;
	UPROPERTY(BlueprintReadWrite)
	float Z;

	FColorXYZ() = default;
	FColorXYZ(float X, float Y, float Z) :X(X), Y(Y), Z(Z) {};
	FColorXYZ(FVector value) :X(value.X), Y(value.Y), Z(value.Z) {};
};


//Unused
//USTRUCT(BlueprintType)
//struct FColorHSV {
//	GENERATED_BODY()
//	UPROPERTY(BlueprintReadWrite)
//	float H;
//	UPROPERTY(BlueprintReadWrite)
//	float S;
//	UPROPERTY(BlueprintReadWrite)
//	float V;
//
//	FColorHSV() = default;
//	FColorHSV(float H, float S, float V) :H(H), S(S), V(V) {};
//	FColorHSV(FVector value) :H(value.X), S(value.Y), V(value.Z) {};
//	FColorHSV(FLinearColor HSV_Linear) :H(HSV_Linear.R), S(HSV_Linear.G), V(HSV_Linear.B) {}; //TODO: Need to test this one
//};

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

// Copyright Epic Games, Inc. All Rights Reserved.

#include "VVAD_ColorUtilsAndWidgetsBPLibrary.h"
#include "VVAD_ColorUtilsAndWidgets.h"

UVVAD_ColorUtilsAndWidgetsBPLibrary::UVVAD_ColorUtilsAndWidgetsBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer) {

}

//Color Space Convertations
FColorXYZ UVVAD_ColorUtilsAndWidgetsBPLibrary::sRGBToXYZcolor(FColor color) {
	//sR, sG and sB (Standard RGB) input range = 0 / 255
	//X, Y and Z output refer to a D65/2deg standard illuminant.

	float var_R = color.R / 255.;
	float var_G = color.G / 255.;
	float var_B = color.B / 255.;

	if (var_R > 0.04045) var_R = pow((var_R + 0.055) / 1.055, 2.4);
	else                 var_R = var_R / 12.92;
	if (var_G > 0.04045) var_G = pow((var_G + 0.055) / 1.055, 2.4);
	else                 var_G = var_G / 12.92;
	if (var_B > 0.04045) var_B = pow((var_B + 0.055) / 1.055, 2.4);
	else                 var_B = var_B / 12.92;

	var_R = var_R * 100;
	var_G = var_G * 100;
	var_B = var_B * 100;

	float X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
	float Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
	float Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

	FColorXYZ NewCol;
	NewCol.X = X;
	NewCol.Y = Y;
	NewCol.Z = Z;

	return NewCol;
}

FColor UVVAD_ColorUtilsAndWidgetsBPLibrary::XYZcolorTosRGB(FColorXYZ XYZcolor) {
	//X, Y and Z input refer to a D65/2deg standard illuminant.
	//sR, sG and sB (standard RGB) output range = 0 / 255

	float X = XYZcolor.X;
	float Y = XYZcolor.Y;
	float Z = XYZcolor.Z;

	float var_X = X / 100.;
	float var_Y = Y / 100.;
	float var_Z = Z / 100.;

	float var_R = var_X * 3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
	float var_G = var_X * -0.9689 + var_Y * 1.8758 + var_Z * 0.0415;
	float var_B = var_X * 0.0557 + var_Y * -0.2040 + var_Z * 1.0570;

	if (var_R > 0.0031308) var_R = 1.055 * pow(var_R, (1. / 2.4)) - 0.055;
	else                   var_R = 12.92 * var_R;
	if (var_G > 0.0031308) var_G = 1.055 * pow(var_G, (1 / 2.4)) - 0.055;
	else                   var_G = 12.92 * var_G;
	if (var_B > 0.0031308) var_B = 1.055 * pow(var_B, (1 / 2.4)) - 0.055;
	else                   var_B = 12.92 * var_B;

	uint8 sR = var_R * 255;
	uint8 sG = var_G * 255;
	uint8 sB = var_B * 255;

	return FColor(sR, sG, sB);
}

FColorCIELUV UVVAD_ColorUtilsAndWidgetsBPLibrary::XYZcolorToCIELUV(FColorXYZ XYZcolor) {
	//Reference X, Y and Z refer to specific illuminants and observers.
	const float ReferenceX = 95.047;
	const float ReferenceY = 100.000;
	const float ReferenceZ = 108.883;

	float X = XYZcolor.X;
	float Y = XYZcolor.Y;
	float Z = XYZcolor.Z;

	float var_U = (4. * X) / (X + (15. * Y) + (3. * Z));
	float var_V = (9. * Y) / (X + (15. * Y) + (3. * Z));

	float var_Y = Y / 100.;
	if (var_Y > 0.008856) var_Y = pow(var_Y, (1. / 3.));
	else                  var_Y = (7.787 * var_Y) + (16. / 116.);

	float ref_U = (4. * ReferenceX) / (ReferenceX + (15. * ReferenceY) + (3. * ReferenceZ));
	float ref_V = (9. * ReferenceY) / (ReferenceX + (15. * ReferenceY) + (3. * ReferenceZ));

	float CIE_L = (116. * var_Y) - 16.;
	float CIE_u = 13. * CIE_L * (var_U - ref_U);
	float CIE_v = 13. * CIE_L * (var_V - ref_V);

	FColorCIELUV NewColor;
	NewColor.L = CIE_L;
	NewColor.u = CIE_u;
	NewColor.v = CIE_v;

	return NewColor;
}

FColorXYZ UVVAD_ColorUtilsAndWidgetsBPLibrary::CIELUVToXYZcolor(FColorCIELUV CIELUV) {
	//Reference-X, Y and Z refer to specific illuminants and observers.
	const float ReferenceX = 95.047;
	const float ReferenceY = 100.000;
	const float ReferenceZ = 108.883;

	float CIE_L = CIELUV.L;
	float CIE_u = CIELUV.u;
	float CIE_v = CIELUV.v;

	float var_Y = (CIE_L + 16.) / 116.;
	if (pow(var_Y, 3) > 0.008856) var_Y = pow(var_Y, 3);
	else                          var_Y = (var_Y - 16. / 116.) / 7.787;

	float ref_U = (4 * ReferenceX) / (ReferenceX + (15 * ReferenceY) + (3 * ReferenceZ));
	float ref_V = (9 * ReferenceY) / (ReferenceX + (15 * ReferenceY) + (3 * ReferenceZ));

	float var_U = CIE_u / (13 * CIE_L) + ref_U;
	float var_V = CIE_v / (13 * CIE_L) + ref_V;

	float Y = var_Y * 100;
	float X = -(9 * Y * var_U) / ((var_U - 4) * var_V - var_U * var_V);
	float Z = (9 * Y - (15 * var_V * Y) - (var_V * X)) / (3 * var_V);

	FColorXYZ NewCol;
	NewCol.X = X;
	NewCol.Y = Y;
	NewCol.Z = Z;

	return NewCol;
}

//Color gradient rendering
UTexture2D* UVVAD_ColorUtilsAndWidgetsBPLibrary::CurveLinearColor_CreateTexture(UObject* WorldContextObject, int32 Width, UCurveLinearColor* Curve, bool bOutputSRGB) {
	if (!Curve || Width <= 0) {
		return nullptr;
	}
	float bMin = 0.0f, bMax = 1.0f;
	Curve->GetTimeRange(bMin, bMax);

	UTexture2D* Texture = UTexture2D::CreateTransient(Width, 1, PF_B8G8R8A8);
	{
		Texture->MipGenSettings = TMGS_NoMipmaps;
		Texture->CompressionSettings = TC_Default;
		Texture->Filter = TF_Bilinear;
		Texture->AddressX = TA_Clamp;
		Texture->AddressY = TA_Clamp;
		Texture->SRGB = bOutputSRGB;
	}

	FTexture2DMipMap& Mip = Texture->PlatformData->Mips[0];   // UE5 Texture->GetPlatformData()->Mips[0]; 
	void* RawData = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FColor* Pixels = static_cast<FColor*>(RawData);

	for (int i = 0; i < Width; i++) {
		const float T = FMath::Lerp(bMin, bMax, float(i) / fmaxf(Width - 1., 1.));
		Pixels[i] = Curve->GetLinearColorValue(T).ToFColor(bOutputSRGB);
	}

	Mip.BulkData.Unlock();
	Texture->UpdateResource();

	return Texture;
}



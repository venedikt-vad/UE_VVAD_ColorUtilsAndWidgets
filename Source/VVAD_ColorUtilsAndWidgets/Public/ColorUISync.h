
#pragma once
#include "ColorUISync.generated.h"

USTRUCT(BlueprintType)
struct FUIColorState {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color = FLinearColor::White;
};

UINTERFACE(BlueprintType)
class UColorUISync : public UInterface {
	GENERATED_BODY()
};

class IColorUISync
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ColorUILink")
	FName GetLinkGroupId() const;

	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ColorUILink")
	//UColorUISync* GetMasterRef();

	// Export current state from THIS widget (master uses this for broadcasting)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Link")
	FUIColorState ExportLinkState() const;

	//// Apply state INTO this widget (slaves use this on master updates)
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Link")
	//void ImportLinkState(const FUIColorState& State);

	//// Called by widget when its state changes due to user interaction
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Link")
	//void NotifyLinkStateChanged();
};

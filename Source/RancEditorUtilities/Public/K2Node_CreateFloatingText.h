// Copyright Rancorous Games, 2024

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_CreateFloatingText.generated.h"

/*
 * This did not really need a K2Node but it was a good exercise and can serve as a reference for reference for others
 */
UCLASS(Category = "Utilities")
class RANCEDITORUTILITIES_API UK2Node_CreateFloatingText : public UK2Node
{
	GENERATED_BODY()
public:
	
	virtual void AllocateDefaultPins() override;

	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetMenuCategory() const override;

	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual FLinearColor GetNodeTitleColor() const override;

	// is node pure
	virtual bool IsNodePure() const override { return false; }
};

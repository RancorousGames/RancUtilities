// Copyright Rancorous Games, 2024
// Credit to https://www.twitch.tv/sovereigndev for the initial implementation and permission to redistribute

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_TimelineObject.generated.h"

/**
 * 
 */
UCLASS()
class RANCEDITORUTILITIES_API UK2Node_TimelineObject : public UK2Node
{
	GENERATED_BODY()

public:
	/* The Logic of this Node, How it gets compiled to have functionality */
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	
		// Function responsible for making the node.
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

		// Visuals
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FLinearColor GetNodeBodyTintColor() const override;

		// Pin Creation & Updating
	virtual void AllocateDefaultPins() override;

	// virtual void PostPlacedNewNode() override;
	
	/* K2 Interface */
	virtual bool IsNodeSafeToIgnore() const override { return true; }

protected:
	static UEnum* GetCurveTypeEnum();
	
	UEdGraphPin* GetPlayPin() const;
	UEdGraphPin* GetReversePin() const;
	UEdGraphPin* GetPlayFromStartPin() const;
	UEdGraphPin* GetReverseFromEndPin() const;

	UEdGraphPin* GetValuePin() const;

	UEdGraphPin* GetDurationPin() const;
	UEdGraphPin* GetCurveTypePin() const;
	
	UEdGraphPin* GetOnUpdatePin() const;
	UEdGraphPin* GetOnFinishedPin() const;
};
// Copyright Rancorous Games, 2024
// Helpful resource on k2nodes in general: https://s1t2.com/blog/brief-intro-k2nodes

#pragma once
#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_CallFunction.h"

namespace StandardPinNames
{
	const FName Source = "Source";
	const FName WorldContextObject = "WorldContextObject";
	const FName Target = "Target";
	const FName Type = "Type";
	const FName ReturnValue = "ReturnValue";
}

// This is a helper struct to make the creation of K2 nodes easier mostly used in the ExpandNode function of a custom K2 node
struct FK2NodeCreationHelper
{
	virtual ~FK2NodeCreationHelper() = default;

protected:
	UK2Node* Node = nullptr;

public:
	template <typename T = FK2NodeCreationHelper>
	static T Create(UK2Node* SourceNode, FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
	{
		T Node;
		Node.SourceNode = SourceNode;
		Node.CompilerContext = &CompilerContext;
		Node.CreateNode(SourceNode, CompilerContext, SourceGraph);
		return Node;
	}

	virtual UEdGraphPin* GetExecPin() const;
	virtual UEdGraphPin* GetThenPin() const;

	void LinkExecAndThenToChildNode() const;

	void LinkPinToChildNode(FName PinName) const;
	
	UK2Node* SourceNode = nullptr;
	FKismetCompilerContext* CompilerContext = nullptr;
	
protected:
	virtual void CreateNode(UK2Node* InSourceNode, FKismetCompilerContext& InCompilerContext, UEdGraph* InSourceGraph)
	{
	}
};

// Specialization of FK2NodeCreationHelper for creating a CallFunctionNode
struct FCallFunctionNodeWrapperHelper : FK2NodeCreationHelper
{
public:
	FName FunctionName;
	TSubclassOf<UObject> ClassToCall;
	UK2Node_CallFunction* CallFunctionNode = nullptr;

	UEdGraphPin* GetReturnPin() const;
	UEdGraphPin* GetTargetPin() const;
	UEdGraphPin* GetPinByName(const FName PinName) const;

	static FCallFunctionNodeWrapperHelper Create(UK2Node* SourceNode, FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, TSubclassOf<UObject> ClassToCall, const FName FunctionName);

	// create a child node calling a specific function
	virtual void CreateNode(UK2Node* InSourceNode, FKismetCompilerContext& InCompilerContext, UEdGraph* InSourceGraph) override;
};
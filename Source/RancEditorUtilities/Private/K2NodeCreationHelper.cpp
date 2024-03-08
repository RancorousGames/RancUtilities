// Copyright Rancorous Games, 2024

#include "K2NodeCreationHelper.h"
#include "KismetCompiler.h"
#include "EdGraphSchema_K2.h"

UEdGraphPin* FK2NodeCreationHelper::GetExecPin() const
{
	return Node->GetExecPin();
}

UEdGraphPin* FK2NodeCreationHelper::GetThenPin() const
{
	return Node->GetThenPin();
}

void FK2NodeCreationHelper::LinkExecAndThenToChildNode() const
{
	CompilerContext->MovePinLinksToIntermediate(*SourceNode->GetExecPin(), *GetExecPin());
	CompilerContext->MovePinLinksToIntermediate(*SourceNode->GetThenPin(), *GetThenPin());
}

void FK2NodeCreationHelper::LinkPinToChildNode(FName PinName) const
{
	CompilerContext->MovePinLinksToIntermediate(*SourceNode->FindPinChecked(PinName), *Node->FindPinChecked(PinName));
}

UEdGraphPin* FCallFunctionNodeWrapperHelper::GetTargetPin() const
{
	return CallFunctionNode->FindPinChecked(UEdGraphSchema_K2::PN_Self);
}

UEdGraphPin* FCallFunctionNodeWrapperHelper::GetPinByName(const FName PinName) const
{
	return CallFunctionNode->FindPinChecked(PinName);
}

FCallFunctionNodeWrapperHelper FCallFunctionNodeWrapperHelper::Create(
	UK2Node* SourceNode, FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, TSubclassOf<UObject> ClassToCall, const FName FunctionName)
{
	FCallFunctionNodeWrapperHelper Node;
	Node.FunctionName = FunctionName;
	Node.ClassToCall = ClassToCall;
	Node.SourceNode = SourceNode;
	Node.CompilerContext = &CompilerContext;
	Node.CreateNode(SourceNode, CompilerContext, SourceGraph);
	return Node;
}

void FCallFunctionNodeWrapperHelper::CreateNode(UK2Node* InSourceNode, FKismetCompilerContext& InCompilerContext, UEdGraph* InSourceGraph)
{
	Node = CallFunctionNode = InCompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(InSourceNode, InSourceGraph);
	CallFunctionNode->FunctionReference.SetExternalMember(FunctionName, ClassToCall);
	CallFunctionNode->AllocateDefaultPins();
}

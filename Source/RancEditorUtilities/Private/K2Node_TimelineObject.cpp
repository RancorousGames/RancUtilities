// Copyright Rancorous Games, 2024
// Credit to https://www.twitch.tv/sovereigndev for the initial implementation and permission to redistribute

#include "K2Node_TimelineObject.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_AddDelegate.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CreateDelegate.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_Self.h"
#include "KismetCompiler.h"
#include "TimelineObject.h"
#include "Kismet/KismetSystemLibrary.h"

#define LOCTEXT_NAMESPACE "K2_TimelineObject"

typedef UEdGraphPin UPin;

namespace PinNames
{
	const FName WorldContextObject = "WorldContextObject";
	const FName Play = "Play";
	const FName PlayFromStart = "PlayFromStart";
	const FName Reverse = "Reverse";
	const FName ReverseFromEnd = "ReverseFromEnd";

	const FName OnUpdated = "OnUpdated";
	const FName OnFinished = "OnFinished";
	
	const FName Value = "Value";

	const FName Duration = "Duration";
	const FName CurveType = "CurveType";
}

struct FNode
{
	virtual ~FNode() = default;
	FNode(){};

protected:
	UK2Node* Node = nullptr;
	
public:
	template<typename T = FNode>
	static T Create(UK2Node* SourceNode, FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
	{
		T Node;
		Node.CreateNode(SourceNode, CompilerContext, SourceGraph);
		return Node;
	}

	virtual UEdGraphPin* GetExecPin() const
	{
		return Node->GetExecPin();
	}
	virtual UEdGraphPin* GetThenPin() const
	{
		return Node->GetThenPin();
	}
	
protected:
	virtual void CreateNode(UK2Node* SourceNode, FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph){}
};

struct FIsValidNode : public FNode
{
	UK2Node_CallFunction* IsValidFuncNode = nullptr;
	UK2Node_IfThenElse* BranchNode = nullptr;
	
	UPin* GetObjectPin() const
	{
		return IsValidFuncNode->FindPinChecked(TEXT("Object"));
	}
	UPin* GetTruePin() const
	{
		return BranchNode->GetThenPin();
	}
	UPin* GetFalsePin() const
	{
		return BranchNode->GetElsePin();
	}

	virtual void CreateNode(UK2Node* SourceNode, FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override
	{
		const FName IsValidFuncName = GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, IsValid);

		IsValidFuncNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SourceNode, SourceGraph);
		IsValidFuncNode->FunctionReference.SetExternalMember(IsValidFuncName, UKismetSystemLibrary::StaticClass());
		IsValidFuncNode->AllocateDefaultPins();
		
		Node = BranchNode = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(SourceNode, SourceGraph);
		BranchNode->AllocateDefaultPins();
		
		CompilerContext.GetSchema()->TryCreateConnection(IsValidFuncNode->GetReturnValuePin(), BranchNode->GetConditionPin());
	}
};

struct FCreateTimelineNode : public FNode
{
	UK2Node_CallFunction* TimelineNode = nullptr;

	UEdGraphPin* GetReturnPin() const
	{
		return TimelineNode->GetReturnValuePin();
	}
	
	virtual void CreateNode(UK2Node* SourceNode, FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override
	{
		static const FName FuncName = GET_FUNCTION_NAME_CHECKED(UTimelineObject, CreateTimeline);

		Node = TimelineNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SourceNode, SourceGraph);
		TimelineNode->FunctionReference.SetExternalMember(FuncName, UTimelineObject::StaticClass());
		TimelineNode->AllocateDefaultPins();
	}
};

struct FSequenceNode : public FNode
{
	UK2Node_ExecutionSequence* SequenceNode = nullptr;

	void AddSequencePin() const
	{
		SequenceNode->AddInputPin();
	}
	UPin* GetSequencePin(const int32 Index) const
	{
		return SequenceNode->GetThenPinGivenIndex(Index);
	}
	
	virtual void CreateNode(UK2Node* SourceNode, FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override
	{
		Node = SequenceNode = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(SourceNode, SourceGraph);
		SequenceNode->AllocateDefaultPins();
	}
};

struct FDelegateEventNode : public FNode
{
	/*
	 * @see UK2Node_BaseAsyncTask::ExpandNode for how to create an actual delegate event node and bind it.
	 *	Specifically the 'HandleDelegates' function that's called @line: 477
	 */
	UK2Node_AddDelegate* AddDelegateNode = nullptr;
	UK2Node_CustomEvent* CustomEventNode = nullptr;

	UPin* GetOnEventPin() const
	{
		return CustomEventNode->GetThenPin();
	}

	FMulticastDelegateProperty* TryFindDelegateProperty() const
	{
		const FName DelegateName = GetDelegateName();
		
		for (TFieldIterator<FMulticastDelegateProperty> PropertyIt(GetOwnerClass()); PropertyIt; ++PropertyIt)
		{
			if(!PropertyIt) continue;
			if(!PropertyIt->HasAnyPropertyFlags(CPF_BlueprintAssignable)) continue;
			if(PropertyIt->GetName() != DelegateName) continue;
				
			return *PropertyIt;
		}

		return nullptr;
	}

	virtual UClass* GetOwnerClass() const
	{
		return UTimelineObject::StaticClass();
	}
	virtual FName GetDelegateName() const
	{
		checkSlow(false);
		return "";
	}

	virtual void ConnectDelegateContextPin(UPin* InPin, FKismetCompilerContext& CompilerContext)
	{
		CompilerContext.GetSchema()->TryCreateConnection(AddDelegateNode->FindPinChecked(UEdGraphSchema_K2::PN_Self), InPin);
	}
	
	virtual void CreateNode(UK2Node* SourceNode, FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override
	{
		const FName DelegateName = GetDelegateName();

		const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

		const FMulticastDelegateProperty* FoundProperty = TryFindDelegateProperty();

		if(!FoundProperty)
		{
			CompilerContext.MessageLog.Error(*FString("Timeline Node could not find delegate property: " + DelegateName.ToString()));
		}
		
		Node = AddDelegateNode = CompilerContext.SpawnIntermediateNode<UK2Node_AddDelegate>(SourceNode, SourceGraph);
		AddDelegateNode->SetFromProperty(FoundProperty, false, GetOwnerClass());
		AddDelegateNode->AllocateDefaultPins();

		CustomEventNode = CompilerContext.SpawnIntermediateNode<UK2Node_CustomEvent>(SourceNode, SourceGraph);
		CustomEventNode->CustomFunctionName = *FString::Printf(TEXT("%s_%s"), *DelegateName.ToString(), *CompilerContext.GetGuid(SourceNode));
		CustomEventNode->AllocateDefaultPins();

		CreateDelegateForNewFunction(AddDelegateNode->GetDelegatePin(), CustomEventNode->GetFunctionName(), SourceNode, SourceGraph, CompilerContext);
		CopyEventSignature(CustomEventNode, AddDelegateNode->GetDelegateSignature(), Schema);

		/*
		* UK2Node_CustomEvent* CurrentCENode = CompilerContext.SpawnIntermediateEventNode<UK2Node_CustomEvent>(CurrentNode, PinForCurrentDelegateProperty, SourceGraph);
		  {
				UK2Node_AddDelegate* AddDelegateNode = CompilerContext.SpawnIntermediateNode<UK2Node_AddDelegate>(CurrentNode, SourceGraph);
				AddDelegateNode->SetFromProperty(CurrentProperty, false, CurrentProperty->GetOwnerClass());
				AddDelegateNode->AllocateDefaultPins();
				bIsErrorFree &= Schema->TryCreateConnection(AddDelegateNode->FindPinChecked(UEdGraphSchema_K2::PN_Self), ProxyObjectPin);
				bIsErrorFree &= Schema->TryCreateConnection(InOutLastThenPin, AddDelegateNode->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
				InOutLastThenPin = AddDelegateNode->FindPinChecked(UEdGraphSchema_K2::PN_Then);
				CurrentCENode->CustomFunctionName = *FString::Printf(TEXT("%s_%s"), *CurrentProperty->GetName(), *CompilerContext.GetGuid(CurrentNode));
				CurrentCENode->AllocateDefaultPins();
			
				bIsErrorFree &= FBaseAsyncTaskHelper::CreateDelegateForNewFunction(AddDelegateNode->GetDelegatePin(), CurrentCENode->GetFunctionName(), CurrentNode, SourceGraph, CompilerContext);
				bIsErrorFree &= FBaseAsyncTaskHelper::CopyEventSignature(CurrentCENode, AddDelegateNode->GetDelegateSignature(), Schema);
		  }
		 */
	}

	bool CreateDelegateForNewFunction(UEdGraphPin* DelegateInputPin, FName FunctionName, UK2Node* CurrentNode, UEdGraph* SourceGraph, FKismetCompilerContext& CompilerContext)
	{
		const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
		check(DelegateInputPin && Schema && CurrentNode && SourceGraph && (FunctionName != NAME_None));
		bool bResult = true;

		// WORKAROUND, so we can create delegate from nonexistent function by avoiding check at expanding step
		// instead simply: Schema->TryCreateConnection(AddDelegateNode->GetDelegatePin(), CurrentCENode->FindPinChecked(UK2Node_CustomEvent::DelegateOutputName));
		UK2Node_Self* SelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(CurrentNode, SourceGraph);
		SelfNode->AllocateDefaultPins();

		UK2Node_CreateDelegate* CreateDelegateNode = CompilerContext.SpawnIntermediateNode<UK2Node_CreateDelegate>(CurrentNode, SourceGraph);
		CreateDelegateNode->AllocateDefaultPins();
		bResult &= Schema->TryCreateConnection(DelegateInputPin, CreateDelegateNode->GetDelegateOutPin());
		bResult &= Schema->TryCreateConnection(SelfNode->FindPinChecked(UEdGraphSchema_K2::PN_Self), CreateDelegateNode->GetObjectInPin());
		CreateDelegateNode->SetFunction(FunctionName);

		return bResult;
	}
	
	bool CopyEventSignature(UK2Node_CustomEvent* CENode, UFunction* Function, const UEdGraphSchema_K2* Schema)
	{
		check(CENode && Function && Schema);

		bool bResult = true;
		for (TFieldIterator<FProperty> PropIt(Function); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
		{
			const FProperty* Param = *PropIt;
			if (!Param->HasAnyPropertyFlags(CPF_OutParm) || Param->HasAnyPropertyFlags(CPF_ReferenceParm))
			{
				FEdGraphPinType PinType;
				bResult &= Schema->ConvertPropertyToPinType(Param, /*out*/ PinType);
				bResult &= (nullptr != CENode->CreateUserDefinedPin(Param->GetFName(), PinType, EGPD_Output));
			}
		}
		return bResult;
	}
};

struct FTimelineObjectOnUpdateNode : FDelegateEventNode
{
	virtual UClass* GetOwnerClass() const override
	{
		return UTimelineObject::StaticClass();
	}
	virtual FName GetDelegateName() const override
	{
		return GET_MEMBER_NAME_CHECKED(UTimelineObject, BP_OnUpdatedDelegate);
	}
};
struct FTimelineObjectOnFinishedNode : FDelegateEventNode
{
	virtual UClass* GetOwnerClass() const override
	{
		return UTimelineObject::StaticClass();
	}
	virtual FName GetDelegateName() const override
	{
		return GET_MEMBER_NAME_CHECKED(UTimelineObject, BP_OnFinishedDelegate);
	}
};

struct FCallFunctionNode : FNode
{
	FName FunctionName;
	UK2Node_CallFunction* CallFunctionNode = nullptr;

	UEdGraphPin* GetReturnPin() const
    {
    	return CallFunctionNode->GetReturnValuePin();
    }

	UEdGraphPin* GetTargetPin() const
	{
		return CallFunctionNode->FindPinChecked(UEdGraphSchema_K2::PN_Self);
	}

	UEdGraphPin* GetPinByName(const FName PinName) const
	{
		return CallFunctionNode->FindPinChecked(PinName);
	}

	static FCallFunctionNode Create(UK2Node* SourceNode, FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, const FName FunctionName)
	{
		FCallFunctionNode Node;
		Node.FunctionName = FunctionName;
		Node.CreateNode(SourceNode, CompilerContext, SourceGraph);
		return Node;
	}

	virtual void CreateNode(UK2Node* SourceNode, FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override
	{
		Node = CallFunctionNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SourceNode, SourceGraph);
		CallFunctionNode->FunctionReference.SetExternalMember(FunctionName, UTimelineObject::StaticClass());
		CallFunctionNode->AllocateDefaultPins();
	}
};

void UK2Node_TimelineObject::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UEdGraphSchema_K2 const* Schema = CompilerContext.GetSchema();

	static const FName CreateTimelineFunctionName = GET_FUNCTION_NAME_CHECKED(UTimelineObject, CreateTimeline);
	static const FName PlayFunctionName = GET_FUNCTION_NAME_CHECKED(UTimelineObject, Play);
	static const FName PlayFromStartFunctionName = GET_FUNCTION_NAME_CHECKED(UTimelineObject, PlayFromStart);
	static const FName ReverseFunctionName = GET_FUNCTION_NAME_CHECKED(UTimelineObject, Reverse);
	static const FName ReverseFromEndFunctionName = GET_FUNCTION_NAME_CHECKED(UTimelineObject, ReverseFromEnd);
	
	FIsValidNode IsValidNode = FNode::Create<FIsValidNode>(this, CompilerContext, SourceGraph);
	
	FCallFunctionNode CreateTimelineNode = FCallFunctionNode::Create(this, CompilerContext, SourceGraph, CreateTimelineFunctionName);

	FCallFunctionNode SetDurationNode = FCallFunctionNode::Create(this, CompilerContext, SourceGraph, GET_FUNCTION_NAME_CHECKED(UTimelineObject, SetDuration));
	FCallFunctionNode SetCurveTypeNode = FCallFunctionNode::Create(this, CompilerContext, SourceGraph, GET_FUNCTION_NAME_CHECKED(UTimelineObject, SetCurveType));

	FCallFunctionNode PlayNode = FCallFunctionNode::Create(this, CompilerContext, SourceGraph, PlayFunctionName);
	FCallFunctionNode PlayFromStartNode = FCallFunctionNode::Create(this, CompilerContext, SourceGraph, PlayFromStartFunctionName);
	FCallFunctionNode ReverseNode = FCallFunctionNode::Create(this, CompilerContext, SourceGraph, ReverseFunctionName);
	FCallFunctionNode ReverseFromEndNode = FCallFunctionNode::Create(this, CompilerContext, SourceGraph, ReverseFromEndFunctionName);

	FSequenceNode PlaySequenceNode = FNode::Create<FSequenceNode>(this, CompilerContext, SourceGraph);
	FSequenceNode ReverseSequenceNode = FNode::Create<FSequenceNode>(this, CompilerContext, SourceGraph);
	FSequenceNode PlayFromStartSequenceNode = FNode::Create<FSequenceNode>(this, CompilerContext, SourceGraph);
	FSequenceNode ReverseFromEndSequenceNode = FNode::Create<FSequenceNode>(this, CompilerContext, SourceGraph);

	PlaySequenceNode.AddSequencePin();
	ReverseSequenceNode.AddSequencePin();
	PlayFromStartSequenceNode.AddSequencePin();
	ReverseFromEndSequenceNode.AddSequencePin();

	FTimelineObjectOnUpdateNode OnUpdateNode = FNode::Create<FTimelineObjectOnUpdateNode>(this, CompilerContext, SourceGraph);
	FTimelineObjectOnFinishedNode OnFinishedNode = FNode::Create<FTimelineObjectOnFinishedNode>(this, CompilerContext, SourceGraph);

	// Connect the Execution Pins
	CompilerContext.MovePinLinksToIntermediate(*GetPlayPin(), *PlaySequenceNode.GetExecPin());
	CompilerContext.MovePinLinksToIntermediate(*GetReversePin(), *ReverseSequenceNode.GetExecPin());
	CompilerContext.MovePinLinksToIntermediate(*GetPlayFromStartPin(), *PlayFromStartSequenceNode.GetExecPin());
	CompilerContext.MovePinLinksToIntermediate(*GetReverseFromEndPin(), *ReverseFromEndSequenceNode.GetExecPin());

	// Connect All Sequence node's first pin to the IsValidNode
	Schema->TryCreateConnection(PlaySequenceNode.GetSequencePin(0), IsValidNode.GetExecPin());
	Schema->TryCreateConnection(ReverseSequenceNode.GetSequencePin(0), IsValidNode.GetExecPin());
	Schema->TryCreateConnection(PlayFromStartSequenceNode.GetSequencePin(0), IsValidNode.GetExecPin());
	Schema->TryCreateConnection(ReverseFromEndSequenceNode.GetSequencePin(0), IsValidNode.GetExecPin());

	// IsValid == False then we need to create the timeline
	Schema->TryCreateConnection(IsValidNode.GetFalsePin(), CreateTimelineNode.GetExecPin());
	Schema->TryCreateConnection(CreateTimelineNode.GetReturnPin(), IsValidNode.GetObjectPin());

	// The Bind the Delegates
	Schema->TryCreateConnection(CreateTimelineNode.GetThenPin(), OnUpdateNode.GetExecPin());
	Schema->TryCreateConnection(OnUpdateNode.GetThenPin(), OnFinishedNode.GetExecPin());
	OnUpdateNode.ConnectDelegateContextPin(CreateTimelineNode.GetReturnPin(), CompilerContext);
	OnFinishedNode.ConnectDelegateContextPin(CreateTimelineNode.GetReturnPin(), CompilerContext);

	// Connect the New Event Nodes to their respective functions
	CompilerContext.MovePinLinksToIntermediate(*GetOnUpdatePin(), *OnUpdateNode.GetOnEventPin());
	CompilerContext.MovePinLinksToIntermediate(*GetOnFinishedPin(), *OnFinishedNode.GetOnEventPin());

	// Connect the float 'Alpha' Output
	CompilerContext.MovePinLinksToIntermediate(*GetValuePin(), *OnUpdateNode.CustomEventNode->FindPinChecked(FName("Alpha")));

	// Copy the Default values for the Duration and CurveType
	if(GetDurationPin()->LinkedTo.IsEmpty()) SetDurationNode.GetPinByName("InDuration")->DefaultValue = GetDurationPin()->DefaultValue;
	else CompilerContext.MovePinLinksToIntermediate(*GetDurationPin(), *SetDurationNode.GetPinByName("InDuration"));
	
	if(GetCurveTypePin()->LinkedTo.IsEmpty()) SetCurveTypeNode.GetPinByName("InCurveType")->DefaultValue = GetCurveTypePin()->DefaultValue;
	else CompilerContext.MovePinLinksToIntermediate(*GetCurveTypePin(), *SetCurveTypeNode.GetPinByName("InCurveType"));
	
	/*
	Schema->TryCreateConnection(GetDurationPin(), SetDurationNode.GetPinByName("InDuration"));
	Schema->TryCreateConnection(GetCurveTypePin(), SetCurveTypeNode.GetPinByName("InCurveType"));
	*/

	// Assign Target to Setters
	Schema->TryCreateConnection(CreateTimelineNode.GetReturnPin(), SetDurationNode.GetTargetPin());
	Schema->TryCreateConnection(CreateTimelineNode.GetReturnPin(), SetCurveTypeNode.GetTargetPin());

	// Connect Setters together
	Schema->TryCreateConnection(SetDurationNode.GetThenPin(), SetCurveTypeNode.GetExecPin());

	// Connect the Sequence Nodes to Setters
	Schema->TryCreateConnection(PlaySequenceNode.GetSequencePin(1), SetDurationNode.GetExecPin());
	Schema->TryCreateConnection(ReverseSequenceNode.GetSequencePin(1), SetDurationNode.GetExecPin());
	Schema->TryCreateConnection(PlayFromStartSequenceNode.GetSequencePin(1), SetDurationNode.GetExecPin());
	Schema->TryCreateConnection(ReverseFromEndSequenceNode.GetSequencePin(1), SetDurationNode.GetExecPin());
	
	// Then Finally Connect the Sequence Nodes to their respective functions
	Schema->TryCreateConnection(PlaySequenceNode.GetSequencePin(2), PlayNode.GetExecPin());
	Schema->TryCreateConnection(ReverseSequenceNode.GetSequencePin(2), ReverseNode.GetExecPin());
	Schema->TryCreateConnection(PlayFromStartSequenceNode.GetSequencePin(2), PlayFromStartNode.GetExecPin());
	Schema->TryCreateConnection(ReverseFromEndSequenceNode.GetSequencePin(2), ReverseFromEndNode.GetExecPin());

	// Assign the Timeline to the PlayNodes
	Schema->TryCreateConnection(CreateTimelineNode.GetReturnPin(), PlayNode.GetTargetPin());
	Schema->TryCreateConnection(CreateTimelineNode.GetReturnPin(), ReverseNode.GetTargetPin());
	Schema->TryCreateConnection(CreateTimelineNode.GetReturnPin(), PlayFromStartNode.GetTargetPin());
	Schema->TryCreateConnection(CreateTimelineNode.GetReturnPin(), ReverseFromEndNode.GetTargetPin());

	BreakAllNodeLinks();
}

FText UK2Node_TimelineObject::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("TimelineTitle", "Timeline");
}

FText UK2Node_TimelineObject::GetMenuCategory() const
{
	return LOCTEXT("TimelineCategory", "Timeline");
}

void UK2Node_TimelineObject::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	Super::GetMenuActions(ActionRegistrar);

	UClass* Action = GetClass();
	
	if (ActionRegistrar.IsOpenForRegistration(Action))
	{
		UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(GetClass());
		check(Spawner != nullptr);

		ActionRegistrar.AddBlueprintAction(Action, Spawner);
	}
}

FSlateIcon UK2Node_TimelineObject::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon(FAppStyle::GetAppStyleSetName(), "EditorViewport.ToggleRealTime");
	return Icon;
}

FLinearColor UK2Node_TimelineObject::GetNodeTitleColor() const
{
	return FLinearColor(0.623529, 0.25098, 0.145098);
}

FLinearColor UK2Node_TimelineObject::GetNodeBodyTintColor() const
{
	return FLinearColor::Gray;
}

void UK2Node_TimelineObject::AllocateDefaultPins()
{
	// Execution Pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, PinNames::Play);
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, PinNames::Reverse);
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, PinNames::PlayFromStart);
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, PinNames::ReverseFromEnd);
	
	// Create Input Variables
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Real, PinNames::Duration)->DefaultValue = FString("1.0");
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Byte, GetCurveTypeEnum(), PinNames::CurveType)->DefaultValue = FString("Linear");
	
	// Output Pins
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinNames::OnUpdated);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinNames::OnFinished);

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Real, PinNames::Value);

	Super::AllocateDefaultPins();
}

UEnum* UK2Node_TimelineObject::GetCurveTypeEnum()
{
	return StaticEnum<ETimelineObjectCurveType>();
}

UEdGraphPin* UK2Node_TimelineObject::GetPlayPin() const
{
	return FindPinChecked(PinNames::Play);
}
UEdGraphPin* UK2Node_TimelineObject::GetReversePin() const
{
	return FindPinChecked(PinNames::Reverse);
}
UEdGraphPin* UK2Node_TimelineObject::GetPlayFromStartPin() const
{
	return FindPinChecked(PinNames::PlayFromStart);
}
UEdGraphPin* UK2Node_TimelineObject::GetReverseFromEndPin() const
{
	return FindPinChecked(PinNames::ReverseFromEnd);
}

UEdGraphPin* UK2Node_TimelineObject::GetValuePin() const
{
	return FindPinChecked(PinNames::Value);
}

UEdGraphPin* UK2Node_TimelineObject::GetDurationPin() const
{
	return FindPinChecked(PinNames::Duration);
}

UEdGraphPin* UK2Node_TimelineObject::GetCurveTypePin() const
{
	return FindPinChecked(PinNames::CurveType);
}

UEdGraphPin* UK2Node_TimelineObject::GetOnUpdatePin() const
{
	return FindPinChecked(PinNames::OnUpdated);
}
UEdGraphPin* UK2Node_TimelineObject::GetOnFinishedPin() const
{
	return FindPinChecked(PinNames::OnFinished);			
}

#undef LOCTEXT_NAMESPACE

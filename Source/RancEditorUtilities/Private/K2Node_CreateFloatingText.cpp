// Copyright Rancorous Games, 2024

#include "K2Node_CreateFloatingText.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "EdGraphSchema_K2.h"
#include "RancUtilityLibrary.h"
#include "BlueprintNodeSpawner.h"
#include "K2NodeCreationHelper.h"
#include "KismetCompiler.h"
#include "Engine/Font.h"

// the custom pins we want for this k2 node, they correspond to the parameters of the function we want to call
namespace PinNames
{
	// text, location, rotation, color, scale, lifetime, font
	const FName TextPin = "Text";
	const FName LocationPin = "Location";
	const FName RotationPin = "Rotation";
	const FName ColorPin = "Color";
	const FName ScalePin = "Scale";
	const FName LifeTimePin = "LifeTime";
	const FName FontPin = "Font";
	const FName AlwaysFaceCameraPin = "AlwaysFaceCamera";
}

// Creates the pins for the node, think of it as the interface of our node
void UK2Node_CreateFloatingText::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	auto* TextPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, PinNames::TextPin);
	TextPin->DefaultValue = "Here!";
	
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, TBaseStructure<FVector>::Get(), PinNames::LocationPin);
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, TBaseStructure<FRotator>::Get(), PinNames::RotationPin);
	auto* ColorPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, TBaseStructure<FColor>::Get(), PinNames::ColorPin);
	ColorPin->DefaultValue = "(R=1.0,G=1.0,B=1.0,A=1.0)";
	auto* ScalePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Real, PinNames::ScalePin);
	ScalePin->DefaultValue = "1.0";
	auto* LifetimePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Real, PinNames::LifeTimePin);
	LifetimePin->DefaultValue = "0.0";
	UEdGraphPin* FontPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UFont::StaticClass(), PinNames::FontPin);

	UFont* DefaultFont = LoadObject<UFont>(nullptr, TEXT("/Script/Engine.Font'/Engine/VREditor/Fonts/VRText_RobotoLarge.VRText_RobotoLarge'"));
	FontPin->DefaultObject = DefaultFont;
	auto* AlwaysFaceCameraPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, PinNames::AlwaysFaceCameraPin);
	AlwaysFaceCameraPin->DefaultValue = "true";
	
	// Example of output
	//CreatePin(EGPD_Output,  UEdGraphSchema_K2::PC_Object, URancUtilityLibrary::StaticClass(), UEdGraphSchema_K2::PN_ReturnValue);
}

// Here we set up the actual functionality of the node
// This is typically done by creating other child/intermediate nodes and connecting them to the external interface of the node
// e.g. the execute pint we get in, we can then connect to the child node, here a CallFunctionNode
void UK2Node_CreateFloatingText::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	// Here we use our helper struct to create a CallFunctionNode and set it up to call the function we want
	const FCallFunctionNodeWrapperHelper Node = FCallFunctionNodeWrapperHelper::Create(this, CompilerContext, SourceGraph,
	                                                                                   URancUtilityLibrary::StaticClass(),
	                                                                                   GET_FUNCTION_NAME_CHECKED(URancUtilityLibrary, CreateFloatingText));

	// Now we need to connect the externale pins such as exec, arguments, return etc to connect to the new node we created
	Node.LinkExecAndThenToChildNode();

	// These two are commonly needed but not for this simple node
	// Node.LinkPinToChildNode(StandardPinNames::Source);
	// Node.LinkPinToChildNode(StandardPinNames::Target);

	// specific for the function we want to call:
	Node.LinkPinToChildNode(PinNames::TextPin);
	Node.LinkPinToChildNode(PinNames::LocationPin);
	Node.LinkPinToChildNode(PinNames::RotationPin);
	Node.LinkPinToChildNode(PinNames::ColorPin);
	Node.LinkPinToChildNode(PinNames::ScalePin);
	Node.LinkPinToChildNode(PinNames::LifeTimePin);
	Node.LinkPinToChildNode(PinNames::FontPin);
	Node.LinkPinToChildNode(PinNames::AlwaysFaceCameraPin);
	
	// If any validation is helpful it can be helpful to call this in case of failure
	// BreakAllNodeLinks();
}

// This is where we register the node so it shows up in the right click menu
void UK2Node_CreateFloatingText::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// here we just register a single action, but e.g. the cast node has many actions when you right click and search cast
	Super::GetMenuActions(ActionRegistrar);

	const UClass* Action = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(Action))
	{
		UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(GetClass());
		check(Spawner);

		ActionRegistrar.AddBlueprintAction(Action, Spawner);
	}
}

FText UK2Node_CreateFloatingText::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Spawn Floating Text Actor");
}

FText UK2Node_CreateFloatingText::GetMenuCategory() const
{
	return FText::FromString("RancUtilities");
}

FSlateIcon UK2Node_CreateFloatingText::GetIconAndTint(FLinearColor& OutColor) const
{
	// To find icons consider getting https://www.unrealengine.com/marketplace/en-US/product/slate-icon-browser
	return FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Edit");
}

FLinearColor UK2Node_CreateFloatingText::GetNodeTitleColor() const
{
	return FLinearColor::MakeFromColorTemperature(2500); // orange
}

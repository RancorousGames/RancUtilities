#include "RancUtilityLibrary.h"
#include "Components/ActorComponent.h"

void URancUtilityLibrary::ForceDestroyComponent(UActorComponent* Component)
{
	if (Component && Component->IsValidLowLevel())
	{
		Component->DestroyComponent();
	}
}

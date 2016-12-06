#include "MTA.h"
#include "InteractionInterface.h"
#include "MTACharacter.h"

bool IInteractionInterface::CanSoldierInteract(AMTACharacter * Character) const
{
	return true;
}

float IInteractionInterface::GetInteractionLength() const
{
	return 0.f;
}

FVector IInteractionInterface::GetInteractionLocation() const
{
	return FVector::ZeroVector;
}

UAnimMontage * IInteractionInterface::GetInteractionMontage(AMTACharacter * Character) const
{
	return NULL;
}

void IInteractionInterface::OnInteract(AMTACharacter * Character)
{

}
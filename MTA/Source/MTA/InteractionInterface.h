#pragma once

#include "InteractionInterface.generated.h"

class AMTACharacter;

UINTERFACE()
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class IInteractionInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual bool CanSoldierInteract(AMTACharacter * Character) const;

	// Return how long a player has to hold down the interaction button until OnInteract is called
	virtual float GetInteractionLength() const;

	// Get location in world space where the origin of the interaction is (player can only interact when close enough to this point)
	virtual FVector GetInteractionLocation() const;

	// Return animations for character to play, can be NULL
	virtual UAnimMontage * GetInteractionMontage(AMTACharacter * Character) const;

	// Do actual interaction logic here
	virtual void OnInteract(AMTACharacter * Character);
};
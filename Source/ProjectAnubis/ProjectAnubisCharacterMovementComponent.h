// Copyright (c) 2026 ne0. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectAnubisCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum class EProjectAnubisCustomMovementMode : uint8
{
	None = 0,
	WallSlide
};

UCLASS()
class PROJECTANUBIS_API UProjectAnubisCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	void PhysCustom(float DeltaSeconds, int32 Iterations) override;
	void ExitWallSlide();

protected:
	void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

private:
	void PhysWallSlide(float DeltaSeconds, int32 Iterations);
};

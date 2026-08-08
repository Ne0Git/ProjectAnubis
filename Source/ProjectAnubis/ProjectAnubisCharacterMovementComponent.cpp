// Copyright (c) 2026 ne0. All Rights Reserved.

#include "ProjectAnubisCharacterMovementComponent.h"

void UProjectAnubisCharacterMovementComponent::PhysCustom(float DeltaSeconds, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case static_cast<uint8>(EProjectAnubisCustomMovementMode::WallSlide):
		PhysWallSlide(DeltaSeconds, Iterations);
		break;
	default:
		Super::PhysCustom(DeltaSeconds, Iterations);
		break;
	}

}

void UProjectAnubisCharacterMovementComponent::PhysWallSlide(float DeltaSeconds, int32 Iterations)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::White, TEXT("PhysWallSlide is ticking"));
	}
}

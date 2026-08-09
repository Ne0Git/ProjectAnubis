// Copyright (c) 2026 ne0. All Rights Reserved.

#include "ProjectAnubisCharacterMovementComponent.h"

namespace {
	float Counter = 0.0f;
	float Duration = 0.0f;
}

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

void UProjectAnubisCharacterMovementComponent::ExitWallSlide()
{
	if (MovementMode == MOVE_Custom && CustomMovementMode == static_cast<uint8>(EProjectAnubisCustomMovementMode::WallSlide))
	{
		Duration = 0.0f;
		SetMovementMode(MOVE_Falling);
	}
}

void UProjectAnubisCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == static_cast<uint8>(EProjectAnubisCustomMovementMode::WallSlide))
	{
		Velocity = FVector::ZeroVector;
	}
}

void UProjectAnubisCharacterMovementComponent::PhysWallSlide(float DeltaSeconds, int32 Iterations)
{
	Counter += DeltaSeconds;
	Duration += DeltaSeconds;
	if (Counter < 1.0f && Duration >= 0.05f) {
		return;
	}
	Counter = 0.0f;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::White, FString::Printf(TEXT("PhysWallSlide is ticking... Duration: %f"), Duration));
	}
}

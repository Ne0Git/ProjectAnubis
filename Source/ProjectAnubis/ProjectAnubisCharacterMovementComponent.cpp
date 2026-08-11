// Copyright (c) 2026 ne0. All Rights Reserved.

#include "ProjectAnubisCharacterMovementComponent.h"

namespace {
	float DebugCounter = 0.0f;
	float DebugDuration = 0.0f;
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
		DebugDuration = 0.0f;
		SetMovementMode(MOVE_Falling);
	}
}

bool UProjectAnubisCharacterMovementComponent::IsWallSlidable(const FVector& SurfaceNormal) const
{
	return SurfaceNormal.Z > -KINDA_SMALL_NUMBER && SurfaceNormal.Z < GetWalkableFloorZ() * 0.5f;
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
	const FVector SlideVelocity = FVector(0.0f, 0.0f, -WallSlideSpeed);
	const FVector Delta = SlideVelocity * DeltaSeconds;
	FHitResult Hit(1.0f);
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.bBlockingHit)
	{
		if (!IsWallSlidable(Hit.ImpactNormal))
		{
			ExitWallSlide();
			return;
		}

		SlideAlongSurface(Delta, 1.0f - Hit.Time, Hit.ImpactNormal, Hit, true);
	}

	DebugCounter += DeltaSeconds;
	DebugDuration += DeltaSeconds;
	if (DebugCounter < 1.0f && DebugDuration >= 0.05f) {
		return;
	}
	DebugCounter = 0.0f;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::White, FString::Printf(TEXT("PhysWallSlide is ticking... Duration: %f"), DebugDuration));
	}
}

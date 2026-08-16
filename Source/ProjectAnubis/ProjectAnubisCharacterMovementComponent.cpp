// Copyright (c) 2026 ne0. All Rights Reserved.

#include "ProjectAnubisCharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"

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

bool UProjectAnubisCharacterMovementComponent::CanStartWallSlide(const FHitResult& Impact) const
{
	if (!IsFalling())
	{
		return false;
	}

	if (Velocity.Size2D() < MinAttachSpeed)
	{
		return false;
	}

	if (!IsWallSlidable(Impact.ImpactNormal))
	{
		return false;
	}

	float DirectionAngleCos = -FVector::DotProduct(Impact.ImpactNormal, CharacterOwner->GetActorForwardVector().GetSafeNormal());
	float MaxAllowedAngleCos = FMath::Cos(FMath::DegreesToRadians(WallAttachAngle));
	if (DirectionAngleCos <= MaxAllowedAngleCos)
	{
		return false;
	}

	return true;
}

void UProjectAnubisCharacterMovementComponent::StartWallSlide(const FVector& SurfaceNormal)
{
	WallSlideNormal = SurfaceNormal;
	Velocity = FVector::ZeroVector;
	CharacterOwner->GetController()->SetIgnoreMoveInput(true);
	SetMovementMode(EMovementMode::MOVE_Custom, static_cast<uint8>(EProjectAnubisCustomMovementMode::WallSlide));
}

void UProjectAnubisCharacterMovementComponent::ExitWallSlide()
{
	if (MovementMode == MOVE_Custom && CustomMovementMode == static_cast<uint8>(EProjectAnubisCustomMovementMode::WallSlide))
	{
		DebugDuration = 0.0f;
		CharacterOwner->GetController()->SetIgnoreMoveInput(false);
		SetMovementMode(MOVE_Falling);
	}
}

bool UProjectAnubisCharacterMovementComponent::IsWallSlidable(const FVector& SurfaceNormal) const
{
	return SurfaceNormal.Z > -KINDA_SMALL_NUMBER && SurfaceNormal.Z < GetWalkableFloorZ() * 0.5f;
}

bool UProjectAnubisCharacterMovementComponent::CanStartWallJump() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == static_cast<uint8>(EProjectAnubisCustomMovementMode::WallSlide);
}

void UProjectAnubisCharacterMovementComponent::StartWallJump()
{
	Velocity = (WallSlideNormal + FVector::UpVector).GetSafeNormal() * JumpZVelocity;

	CharacterOwner->LaunchCharacter(Velocity, true, true);
	ExitWallSlide();
}

void UProjectAnubisCharacterMovementComponent::PhysWallSlide(float DeltaSeconds, int32 Iterations)
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start - WallSlideNormal * WallCheckDistance;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(CharacterOwner);

	FHitResult WallHit;

	if (!GetWorld()->LineTraceSingleByChannel(WallHit, Start, End, ECC_Visibility, QueryParams))
	{
		ExitWallSlide();
		return;
	}

	Velocity = FVector(0.0f, 0.0f, -WallSlideSpeed);
	const FVector Delta = Velocity * DeltaSeconds;
	FHitResult MoveHit;
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, MoveHit);

	if (MoveHit.bBlockingHit)
	{
		if (!IsWallSlidable(MoveHit.ImpactNormal))
		{
			ExitWallSlide();
			return;
		}

		SlideAlongSurface(Delta, 1.0f - MoveHit.Time, MoveHit.ImpactNormal, MoveHit, true);
	}

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.0f, 0, 2.0f);
	DrawDebugCapsule(
		GetWorld(),
		UpdatedComponent->GetComponentLocation(),
		CharacterOwner->GetSimpleCollisionHalfHeight(),
		CharacterOwner->GetSimpleCollisionRadius(),
		UpdatedComponent->GetComponentQuat(),
		FColor::Yellow,
		false,
		0.0f
	);

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

// Copyright (c) 2026 ne0. All Rights Reserved.

#include "ProjectAnubisCharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "ProjectAnubisCharacter.h"

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
	case static_cast<uint8>(EProjectAnubisCustomMovementMode::WallRun):
		PhysWallRun(DeltaSeconds, Iterations);
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

	const float DirectionAngleCos = -FVector::DotProduct(Impact.ImpactNormal, CharacterOwner->GetActorForwardVector().GetSafeNormal());
	const float MaxAllowedAngleCos = FMath::Cos(FMath::DegreesToRadians(WallAttachAngle));
	if (DirectionAngleCos <= MaxAllowedAngleCos)
	{
		return false;
	}

	return true;
}

void UProjectAnubisCharacterMovementComponent::StartWallSlide(const FVector& SurfaceNormal)
{
	WallNormal = SurfaceNormal;
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
	return MovementMode == MOVE_Custom
		&& (CustomMovementMode == static_cast<uint8>(EProjectAnubisCustomMovementMode::WallSlide)
		|| CustomMovementMode == static_cast<uint8>(EProjectAnubisCustomMovementMode::WallRun));
}

void UProjectAnubisCharacterMovementComponent::StartWallJump()
{
	Velocity = (WallNormal + FVector::UpVector).GetSafeNormal() * JumpZVelocity;

	const bool bIsWallRunning = CustomMovementMode == static_cast<uint8>(EProjectAnubisCustomMovementMode::WallRun);
	if (bIsWallRunning)
	{
		Velocity += GetWallRunDirection() * JumpZVelocity;
	}

	CharacterOwner->LaunchCharacter(Velocity, true, true);

	if (bIsWallRunning)
	{
		ExitWallRun();
	}
	else
	{
		ExitWallSlide();
	}
}

bool UProjectAnubisCharacterMovementComponent::CanStartWallRun(const FHitResult& Impact) const
{
	if (!IsFalling())
	{
		return false;
	}

	if (Velocity.Size2D() < MinAttachSpeed)
	{
		return false;
	}

	if (!IsWallRunable(Impact.ImpactNormal))
	{
		return false;
	}

	const FVector CameraRight = ProjectAnubisCharacter->GetFollowCamera()->GetRightVector();
	const float WallSideFactor = FMath::Abs(FVector::DotProduct(Impact.ImpactNormal, CameraRight));

	if (WallSideFactor < MinWallSideFactor)
	{
		return false;
	}

	return IsWallRunInputPresent(GetWallSide(Impact.ImpactNormal));
}

void UProjectAnubisCharacterMovementComponent::StartWallRun(const FVector& SurfaceNormal)
{
	WallNormal = SurfaceNormal;
	SetMovementMode(EMovementMode::MOVE_Custom, static_cast<uint8>(EProjectAnubisCustomMovementMode::WallRun));
}

void UProjectAnubisCharacterMovementComponent::ExitWallRun()
{
	if (MovementMode == MOVE_Custom && CustomMovementMode == static_cast<uint8>(EProjectAnubisCustomMovementMode::WallRun))
	{
		DebugDuration = 0.0f;
		SetMovementMode(MOVE_Falling);
	}
}

bool UProjectAnubisCharacterMovementComponent::IsWallRunable(const FVector& SurfaceNormal) const
{
	return SurfaceNormal.Z > -KINDA_SMALL_NUMBER && SurfaceNormal.Z < GetWalkableFloorZ();
}

void UProjectAnubisCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	ProjectAnubisCharacter = Cast<AProjectAnubisCharacter>(CharacterOwner);
	check(ProjectAnubisCharacter);
}

void UProjectAnubisCharacterMovementComponent::PhysWallSlide(float DeltaSeconds, int32 Iterations)
{
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start - WallNormal * WallCheckDistance;
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

void UProjectAnubisCharacterMovementComponent::PhysWallRun(float DeltaSeconds, int32 Iterations)
{
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start - WallNormal * WallCheckDistance;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(CharacterOwner);

	FHitResult WallHit;

	if (!GetWorld()->LineTraceSingleByChannel(WallHit, Start, End, ECC_Visibility, QueryParams))
	{
		GEngine->AddOnScreenDebugMessage(4, 1.0, FColor::Red, TEXT("Exiting by LineTrace fail"));
		ExitWallRun();
		return;
	}

	WallNormal = WallHit.ImpactNormal;

	const FVector CameraRight = ProjectAnubisCharacter->GetFollowCamera()->GetRightVector();
	const float WallSideFactor = FMath::Abs(FVector::DotProduct(WallNormal, CameraRight));

	if (WallSideFactor < MinWallSideFactor)
	{
		GEngine->AddOnScreenDebugMessage(4, 1.0, FColor::Red, TEXT("Exiting by WallSideFactor fail"));
		ExitWallRun();
		return;
	}

	if (!IsWallRunInputPresent(GetWallSide(WallNormal)))
	{
		GEngine->AddOnScreenDebugMessage(4, 1.0, FColor::Red, TEXT("Exiting by InputPresent fail"));
		ExitWallRun();
		return;
	}

	const FVector WallRunDirection = GetWallRunDirection();

	Velocity = WallRunDirection * GetMaxSpeed();
	const FVector Delta = Velocity * DeltaSeconds;
	FHitResult MoveHit;
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, MoveHit);

	if (MoveHit.bBlockingHit)
	{
		const float WallNormalFactor = FVector::DotProduct(WallNormal, MoveHit.ImpactNormal);

		if (WallNormalFactor < MinWallNormalFactor)
		{
			GEngine->AddOnScreenDebugMessage(4, 1.0, FColor::Red, TEXT("Exiting by WallNormalFactor in Blocking Hit"));
			ExitWallRun();
			return;
		}

		WallNormal = MoveHit.ImpactNormal;
	}

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.0f, 0, 2.0f);
	DrawDebugCapsule(
		GetWorld(),
		UpdatedComponent->GetComponentLocation(),
		CharacterOwner->GetSimpleCollisionHalfHeight(),
		CharacterOwner->GetSimpleCollisionRadius(),
		UpdatedComponent->GetComponentQuat(),
		FColor::Blue,
		false,
		0.0f
	);

	const FVector PEnd = Start + WallRunDirection * WallCheckDistance;
	DrawDebugLine(GetWorld(), Start, PEnd, FColor::Cyan, false, 0.0f, 0, 2.0f);

	const FVector NEnd = Start - WallRunDirection * WallCheckDistance;
	DrawDebugLine(GetWorld(), Start, NEnd, FColor::Emerald, false, 0.0f, 0, 2.0f);

	DebugCounter += DeltaSeconds;
	DebugDuration += DeltaSeconds;
	if (DebugCounter < 1.0f && DebugDuration >= 0.05f) {
		return;
	}
	DebugCounter = 0.0f;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Blue, FString::Printf(TEXT("PhysWallRun is ticking... Duration: %f"), DebugDuration));
	}
}

EWallSide UProjectAnubisCharacterMovementComponent::GetWallSide(const FVector& SurfaceNormal) const
{
	if (FVector::DotProduct(SurfaceNormal, ProjectAnubisCharacter->GetFollowCamera()->GetRightVector()) > KINDA_SMALL_NUMBER)
	{
		return EWallSide::Left;
	}

	if (FVector::DotProduct(SurfaceNormal, ProjectAnubisCharacter->GetFollowCamera()->GetRightVector()) < -KINDA_SMALL_NUMBER)
	{
		return EWallSide::Right;
	}

	return EWallSide::None;
}

bool UProjectAnubisCharacterMovementComponent::IsWallRunInputPresent(EWallSide Side) const
{
	if (GEngine)
	{
		if (Side == EWallSide::Right)
		{
			GEngine->AddOnScreenDebugMessage(1, 1.0, FColor::Cyan, TEXT("Wall is on Right"));
		}
		else if (Side == EWallSide::Left)
		{
			GEngine->AddOnScreenDebugMessage(1, 1.0, FColor::Magenta, TEXT("Wall is on Left"));
		}
	}

	const float ForwardAxis = ProjectAnubisCharacter->GetForwardAxisValue();

	if (ForwardAxis < 0.1f && ForwardAxis > -0.1f)
	{
		return false;
	}

	const float RightAxis = ProjectAnubisCharacter->GetRightAxisValue();
	
	if (Side == EWallSide::Right && RightAxis < 0.1f)
	{
		return false;
	}

	if (Side == EWallSide::Left && RightAxis > -0.1f)
	{
		return false;
	}

	return true;
}

const FVector UProjectAnubisCharacterMovementComponent::GetWallRunDirection() const
{
	const float DirectionSign = -FVector::DotProduct(CharacterOwner->GetActorRightVector(), WallNormal);
	return (FVector::CrossProduct(FVector::UpVector, WallNormal) * DirectionSign).GetSafeNormal();
}

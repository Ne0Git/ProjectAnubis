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

	bool CanStartWallSlide(const FHitResult& Impact) const;
	void StartWallSlide(const FVector& SurfaceNormal);
	void ExitWallSlide();
	bool IsWallSlidable(const FVector& SurfaceNormal) const;

	bool CanStartWallJump() const;
	void StartWallJump();

private:
	void PhysWallSlide(float DeltaSeconds, int32 Iterations);

private:
	// Maximum angle (in degrees) between the wall surface and a vertical plane that still allows the character to attach.
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Wall Slide", meta = (ClampMin = "0", ClampMax = "90"))
	float WallAttachAngle = 30.0f;

	// Minimum horizontal movement speed required to attach to a wall.
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Wall Slide", meta = (ClampMin = "0"))
	float MinAttachSpeed = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Wall Slide", meta = (ClampMin = "0"))
	float WallSlideSpeed = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Wall Slide", meta = (ClampMin = "0"))
	float WallCheckDistance = 100.0f;

	FVector WallSlideNormal = FVector::ZeroVector;
};

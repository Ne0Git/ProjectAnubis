// Copyright (c) 2026 ne0. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectAnubisCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum class EProjectAnubisCustomMovementMode : uint8
{
	None = 0,
	WallSlide,
	WallRun
};

UENUM(BlueprintType)
enum class EWallSide : uint8
{
	None = 0,
	Left,
	Right
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

	bool CanStartWallRun(const FHitResult& Impact) const;
	void StartWallRun(const FVector& SurfaceNormal);
	void ExitWallRun();
	bool IsWallRunable(const FVector& SurfaceNormal) const;

	void BeginPlay() override;

private:
	void PhysWallSlide(float DeltaSeconds, int32 Iterations);
	void PhysWallRun(float DeltaSeconds, int32 Iterations);

	EWallSide GetWallSide(const FVector& SurfaceNormal) const;
	bool IsWallRunInputPresent(EWallSide Side) const;

	const FVector GetWallRunDirection() const;

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

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Wall Run", meta = (ClampMin = "0"))
	float MinWallSideFactor = 0.6f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Wall Run", meta = (ClampMin = "0"))
	float MinWallNormalFactor = 0.7f;

	FVector WallNormal = FVector::ZeroVector;

	const class AProjectAnubisCharacter* ProjectAnubisCharacter = nullptr;
};

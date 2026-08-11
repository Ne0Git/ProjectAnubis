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
	void StartWallSlide(const FVector& SurfaceNormal);
	void ExitWallSlide();
	bool IsWallSlidable(const FVector& SurfaceNormal) const;

private:
	void PhysWallSlide(float DeltaSeconds, int32 Iterations);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Wall Slide", meta = (ClampMin = "0"))
	float WallSlideSpeed = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Wall Slide", meta = (ClampMin = "0"))
	float WallCheckDistance = 100.0f;

	FVector WallSlideNormal = FVector::ZeroVector;
};

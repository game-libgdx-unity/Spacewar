// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PaperCharacter.h"
#include "SketchCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SKETCHWARS_API ASketchCharacter : public APaperCharacter
{
	GENERATED_BODY()

private:
	FTransform InitialTransform;
	bool IsRespawning;
	int32 RespawnCountdownTime;
	FTimerHandle RespawnCountdownTimerHandle;
	float CurrentAlpha;
	bool FadingIn;
	
public:
	// Sets default values for this character's properties
	ASketchCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UFUNCTION()
	void TurnRight(float val);

	UFUNCTION()
	void MoveUp(float val);

	UFUNCTION()
	void Fire();

	// Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ABullet> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = Flipbooks)
	class UPaperFlipbook* IdleFlipbook;

	UPROPERTY(EditDefaultsOnly, Category = Flipbooks)
	class UPaperFlipbook* AccelerateFlipbook;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lives")
	int32 NumLives;

	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* Actor, class UPrimitiveComponent* Other, FVector Impulse, const FHitResult & HitResult);

	UFUNCTION()
	void Respawn();
	void AdvanceRespawnTimer();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	int32 Score;

	inline void SetScore(int32 Score) { this->Score = Score; }
	inline void IncrementScore(int32 Increment) { this->Score += Increment; }
};

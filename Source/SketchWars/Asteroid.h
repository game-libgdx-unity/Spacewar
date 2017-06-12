// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Asteroid.generated.h"

// Define the size of the asteroids
enum class Size { LARGE, MEDIUM, SMALL };

UCLASS()
class SKETCHWARS_API AAsteroid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsteroid();
	AAsteroid(Size size);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void SpawnFragments(FRotator rot);

	// Sphere collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComponent;

	// Projectile movement component.
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, Category = Movement)
	float RotationRate;

	UPROPERTY(VisibleAnywhere, Category = "Sprites")
	class UPaperSpriteComponent* SpriteComponent;

	UPROPERTY(VisibleAnywhere, Category = "Scoring")
	int32 ScoreValue;

	void SetSize(Size size);

	// Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = Asteroids)
	TSubclassOf<class AAsteroid> RespawnClass;

	// Function that is called when the projectile hits something.
	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* Actor, class UPrimitiveComponent* Other, FVector Impulse, const FHitResult & HitResult);

private:
	Size AsteroidSize;
};

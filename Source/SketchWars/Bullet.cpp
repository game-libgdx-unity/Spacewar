// Fill out your copyright notice in the Description page of Project Settings.

#include "SketchWars.h"
#include "Bullet.h"


// Sets default values
ABullet::ABullet() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation.
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Bullet"));
	CollisionComponent->InitBoxExtent(FVector(5.0f, 0.0f, 5.0f));
	RootComponent = CollisionComponent;

	// Use this component to drive this projectile's movement.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 1000.0f;
	ProjectileMovementComponent->MaxSpeed = 1000.0f;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->ProjectileGravityScale = 0;

	// Die after 1 seconds.
	InitialLifeSpan = 0.5f;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ABullet::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );

	auto location = GetActorLocation();
	FVector bounds, origin;
	GetActorBounds(false, origin, bounds);
	auto halfWidth = (bounds.X / 2);
	auto halfHeight = (bounds.Z / 2);

	if (location.X + halfWidth < 0.0f) {
		location.X = 1024.0f + halfWidth;
	} else if (location.X > 1024.0f + halfWidth) {
		location.X = 0 - halfWidth;
	} else if (location.Z + halfHeight < 0.0f) {
		location.Z = 768.0f + halfHeight;
	} else if (location.Z > 768.0f + halfHeight) {
		location.Z = 0 - halfHeight;
	}
	SetActorLocation(location, false, nullptr, ETeleportType::TeleportPhysics);
}

// Function that initializes the projectile's velocity in the shoot direction.
void ABullet::FireInDirection(const FVector& ShootDirection) {
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}
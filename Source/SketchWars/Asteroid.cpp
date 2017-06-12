// Fill out your copyright notice in the Description page of Project Settings.

#include "SketchWars.h"
#include "Asteroid.h"
#include "Bullet.h"
#include "SketchCharacter.h"

#include "PaperSpriteComponent.h"

// Sets default values
AAsteroid::AAsteroid(Size size) : AsteroidSize(size) {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation.
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Asteroid"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &AAsteroid::OnHit);
	CollisionComponent->InitSphereRadius(15.0f);
	RootComponent = CollisionComponent;
	
	// Set up movement
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 100.0f;
	ProjectileMovementComponent->MaxSpeed = 300.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
	RotationRate = -0.25f;

	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
	SpriteComponent->AttachTo(RootComponent);

	ScoreValue = 10;
}

AAsteroid::AAsteroid() : AAsteroid(Size::LARGE) {}

// Called when the game starts or when spawned
void AAsteroid::BeginPlay() {
	Super::BeginPlay();
}

void AAsteroid::SpawnFragments(FRotator rot) {
	UWorld* World = GetWorld();

	if (RespawnClass && World) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		FRotator Rotation1 = rot.Add(90, 0, 0);
		FRotator Rotation2 = FRotator(rot).Add(-90, 0, 0);

		auto NewAsteroid1 = World->SpawnActor<AAsteroid>(RespawnClass, this->GetActorLocation(), Rotation1, SpawnParams);
		auto NewAsteroid2 = World->SpawnActor<AAsteroid>(RespawnClass, this->GetActorLocation(), Rotation2, SpawnParams);
	}
}

// Called every frame
void AAsteroid::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );

	auto location = GetActorLocation();
	auto halfWidth = CollisionComponent->GetScaledSphereRadius()*1.75;

	if (location.X + halfWidth < 0.0f) {
		location.X = 1024.0f + halfWidth;
	} else if (location.X > 1024.0f + halfWidth) {
		location.X = 0 - halfWidth;
	} else if (location.Z + halfWidth < 0.0f) {
		location.Z = 768.0f + halfWidth;
	} else if (location.Z > 768.0f + halfWidth) {
		location.Z = 0 - halfWidth;
	}
	SetActorLocation(location, false, nullptr, ETeleportType::TeleportPhysics);

	auto rotator = FRotator::MakeFromEuler(FVector(0.0f, RotationRate, 0.0f));
	SpriteComponent->AddWorldRotation(rotator);
}

void AAsteroid::SetSize(Size size) {
	AsteroidSize = size;
}

void AAsteroid::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector Impulse, const FHitResult & HitResult) {
	if (OtherActor != this && OtherActor->IsA(ABullet::StaticClass())) {
		auto Player = Cast<ASketchCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		auto Bullet = Cast<ABullet>(OtherActor);
		Player->IncrementScore(ScoreValue);
		SpawnFragments(Bullet->GetActorRotation());
		Bullet->Destroy();
		Destroy();
	}
}
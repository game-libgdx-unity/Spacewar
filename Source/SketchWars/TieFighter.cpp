// Fill out your copyright notice in the Description page of Project Settings.

#include "SketchWars.h"
#include "TieFighter.h"
#include "PaperSpriteComponent.h"
#include "Bullet.h"
#include "SketchCharacter.h"


// Sets default values
ATieFighter::ATieFighter() : RemainingFireTime(100) {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation.
	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
	SpriteComponent->OnComponentHit.AddDynamic(this, &ATieFighter::OnHit);
	RootComponent = SpriteComponent;

	// Use this component to drive this projectile's movement.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(SpriteComponent);
	ProjectileMovementComponent->InitialSpeed = 150.0f;
	ProjectileMovementComponent->MaxSpeed = 150.0f;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->ProjectileGravityScale = 0;
}

// Called when the game starts or when spawned
void ATieFighter::BeginPlay() {
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATieFighter::AdvanceFireTimer, 0.01f, true);
}

// Called every frame
void ATieFighter::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );
	auto Loc = GetActorLocation();
	FVector Bounds, Origin;
	GetActorBounds(false, Origin, Bounds);

	if (Loc.X >= (1024.0f + Bounds.X) || Loc.X <= (0.0f - Bounds.X)) {
		this->Destroy();
	}
}

void ATieFighter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector Impulse, const FHitResult & HitResult) {
	if (OtherActor != this && OtherActor->IsA(ABullet::StaticClass())) {
		auto Player = Cast<ASketchCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		auto Bullet = Cast<ABullet>(OtherActor);
		Player->IncrementScore(ScoreValue);
		Bullet->Destroy();
		this->Destroy();
	}
}

void ATieFighter::AdvanceFireTimer() {
	RemainingFireTime--;

	if (RemainingFireTime <= 0) {
		RemainingFireTime = FMath::RandRange(100, 200);
		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		FVector Normal = (Character->GetActorLocation() - GetActorLocation());
		auto RandAngle = FMath::RandBool() ? FMath::RandRange(6, 12) : FMath::RandRange(-12, -6);

		Normal.Normalize();
		FireInDirection(Normal.RotateAngleAxis(RandAngle, GetActorRightVector()));
		GetWorldTimerManager().ClearTimer(FireTimerHandle);
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATieFighter::AdvanceFireTimer, 0.01f, true);
	}
}

void ATieFighter::FireInDirection(const FVector& ShootDirection) {
	// Attempt to fire a projectile.
	if (ProjectileClass) {
		UWorld* World = GetWorld();
		if (World) {
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			// Spawn the projectile at the muzzle.
			ABullet* Projectile = World->SpawnActor<ABullet>(ProjectileClass, GetActorLocation(), GetActorRotation(), SpawnParams);
			if (Projectile) {
				// Set the projectile's initial trajectory.
				Projectile->FireInDirection(ShootDirection);
			}
		}
	}
}
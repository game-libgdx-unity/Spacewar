// Fill out your copyright notice in the Description page of Project Settings.

#include "SketchWars.h"
#include "SketchCharacter.h"
#include "Bullet.h"
#include "Asteroid.h"
#include "PaperFlipbookComponent.h"

// Sets default values
ASketchCharacter::ASketchCharacter() :
	NumLives(3),
	IsRespawning(false),
	RespawnCountdownTime(360) {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASketchCharacter::OnHit);
}

// Called when the game starts or when spawned
void ASketchCharacter::BeginPlay() {
	Super::BeginPlay();
	InitialTransform = FTransform(GetActorTransform());
}

// Called every frame
void ASketchCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	auto location = GetActorLocation();
	FVector bounds, origin;
	GetActorBounds(false, origin, bounds);
	auto halfWidth = (bounds.X / 2);
	auto halfHeight = (bounds.Z / 2);

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, location.ToString());
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

// Called to bind functionality to input
void ASketchCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent) {
	Super::SetupPlayerInputComponent(InputComponent);

	// Set up "movement" bindings.
	InputComponent->BindAxis("TurnRight", this, &ASketchCharacter::TurnRight);
	InputComponent->BindAxis("Accelerate", this, &ASketchCharacter::MoveUp);

	// Set up 
	InputComponent->BindAction("Fire", IE_Pressed, this, &ASketchCharacter::Fire);
}

void ASketchCharacter::TurnRight(float val) {
	auto localTransform = GetActorTransform();
	auto rotator = FRotator::MakeFromEuler(FVector(0.0f, val*-2.0f, 0.0f));
	localTransform.ConcatenateRotation(rotator.Quaternion());
	SetActorTransform(FTransform(localTransform));
}

void ASketchCharacter::MoveUp(float val) {
	GetSprite()->SetFlipbook((val != 0.0f) ? AccelerateFlipbook : IdleFlipbook);
	AddMovementInput(GetActorUpVector(), val*0.5f);
}

void ASketchCharacter::Fire() {
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
				Projectile->FireInDirection(GetActorUpVector());
			}
		}
	}
}

void ASketchCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector Impulse, const FHitResult & HitResult) {
	if (OtherActor != this) {
		if (OtherActor->IsA(ABullet::StaticClass())) {
			OtherActor->Destroy();
		}

		if (!IsRespawning) {
			this->Respawn();
		}
	}
}

void ASketchCharacter::Respawn() {
	NumLives--;
	UWorld* World = GetWorld();

	if (World) {
		TArray<AActor*> FoundPlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), FoundPlayerStarts);

		GetMovementComponent()->StopMovementImmediately();
		SetActorTransform(InitialTransform);
		IsRespawning = true;
		CurrentAlpha = 1.0f;
		FadingIn = false;
		GetWorldTimerManager().SetTimer(RespawnCountdownTimerHandle, this, &ASketchCharacter::AdvanceRespawnTimer, 0.01f, true);
	}
}

void ASketchCharacter::AdvanceRespawnTimer() {
	CurrentAlpha += (FadingIn ? 0.02f : -0.02f);
	if (CurrentAlpha >= 1.0f || CurrentAlpha <= 0.5f) {
		FadingIn = !FadingIn;
	}
	GetSprite()->SetSpriteColor(FLinearColor(1.0f, 1.0f, 1.0f, CurrentAlpha));

	RespawnCountdownTime--;
	if (RespawnCountdownTime <= 0) {
		IsRespawning = false;
		RespawnCountdownTime = 360;
		GetWorldTimerManager().ClearTimer(RespawnCountdownTimerHandle);
		GetSprite()->SetSpriteColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	}
}
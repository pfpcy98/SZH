// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieHorde.h"
#include "Turret.h"


AZombieHorde::AZombieHorde()
{
 	PrimaryActorTick.bCanEverTick = true;

	rootBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("rootBox"));
	RootComponent = rootBoxComponent;
	rootBoxComponent->SetBoxExtent(FVector(175.0f, 175.0f, 175.0f));

	audioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	audioComponent->AttachTo(RootComponent);

	// Mesh Loading
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> norZombieMesh1(
		TEXT("/Game/Model/Zombie/Meshes/Normal_Zombie.SK_Zombie_Businessman_Male_01")
	);

	if (norZombieMesh1.Succeeded())
	{
		normalZombieMesh[0] = norZombieMesh1.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> norZombieMesh2(
		TEXT("/Game/Model/Zombie/Meshes/Normal_Zombie1.SK_Zombie_BioHazardSuit_Male_01")
	);

	if (norZombieMesh2.Succeeded())
	{
		normalZombieMesh[1] = norZombieMesh2.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> norZombieMesh3(
		TEXT("/Game/Model/Zombie/Meshes/Normal_Zombie2.SK_Zombie_Jacket_Male_01")
	);

	if (norZombieMesh3.Succeeded())
	{
		normalZombieMesh[2] = norZombieMesh3.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> norZombieMesh4(
		TEXT("/Game/Model/Zombie/Meshes/Normal_Zombie3.SK_Zombie_Patient_Female_01")
	);

	if (norZombieMesh4.Succeeded())
	{
		normalZombieMesh[3] = norZombieMesh4.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> bosZombieMesh(
		TEXT("/Game/Model/Zombie/Meshes/Boss_Zombie.SK_Zombie_HotDogSuit_Male_01")
	);

	if (bosZombieMesh.Succeeded())
	{
		bossZombieMesh = bosZombieMesh.Object;
	}

	// Anim Instance Loading
	static ConstructorHelpers::FClassFinder<UAnimInstance> zombieAnim(
		TEXT("/Game/Created/ZombieAnimBlueprint"));

	if (zombieAnim.Succeeded())
	{
		zombieAnimInstance = zombieAnim.Class;
	}

	// Sound Loading
	static ConstructorHelpers::FObjectFinder<USoundBase> attackSound(
		TEXT("/Game/Sound/Zombie/Zombie_Male_attack1.Zombie_Male_attack1")
	);

	if (attackSound.Succeeded())
	{
		zombieAttackSound = attackSound.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> deathSound(
		TEXT("/Game/Sound/Zombie/Zombie_Death.Zombie_hit_1")
	);

	if (deathSound.Succeeded())
	{
		zombieDeathSound = deathSound.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> idleSound(
		TEXT("/Game/Sound/Zombie/Zombie_Idle.Zombie_Male_tense3")
	);

	if (idleSound.Succeeded())
	{
		zombieIdleSound = idleSound.Object;
	}

	// Particle Loading
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> deathParticle(
		TEXT("/Game/FXVillesBloodVFXPack/Particles/PS_Blood_Burst_Omni.PS_Blood_Burst_Omni")
	);

	if (deathParticle.Succeeded())
	{
		zombieDeathParticle = deathParticle.Object;
	}

	zombiePos[0] = FVector(0.0f, 100.0f, -164.0f);
	zombiePos[1] = FVector(0.0f, -100.0f, -164.0f);
	zombiePos[2] = FVector(100.0f, 0.0f, -164.0f);
	zombiePos[3] = FVector(-100.0f, 0.0f, -164.0f);

	is_Attacked = false;
	idleSoundCooldown = 2.0f;

	Tags.Add("Zombie");
}

void AZombieHorde::BeginPlay()
{
	Super::BeginPlay();
	
	rootBoxComponent->SetCollisionProfileName(FName("BlockAll"));
	rootBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AZombieHorde::OnOverlapBegin);
	rootBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AZombieHorde::OnOverlapEnd);

	positionToMove.Z = GetActorLocation().Z;

	AGameModeBase* gamemode = UGameplayStatics::GetGameMode(GetWorld());
	if (::IsValid(gamemode))
	{
		managerInstance = Cast<ASZHGameModeBase>(gamemode);
	}

	UE_LOG(LogTemp, Warning, TEXT("Spawn"));
}

void AZombieHorde::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SightCheck();

	if (!is_StuckByZombie && !is_StuckByDoor)
	{
		MoveFowardToTarget(DeltaTime);
	}
	else
	{
		if (status == EZombieStatus::E_ATTACKREADY &&
			targetDoor != nullptr &&
			attackCooldown <= 0.0f)
		{
			status = EZombieStatus::E_ATTACK;
		}

		else if (status == EZombieStatus::E_MOVE)
		{
			StateChangeToIdle();
		}

		else if (status == EZombieStatus::E_ATTACK
			&& attackCooldown > 0.0f)
		{
			status = EZombieStatus::E_ATTACKREADY;
		}

		else if(targetDoor != nullptr && !targetDoor->ActorHasTag(FName("Door")))
		{
			is_StuckByDoor = false;
			StateChangeToIdle();
		}
	}

	if (status == EZombieStatus::E_IDLE)
	{
		if (idleSoundCooldown <= 0.0f)
		{
			audioComponent->SetSound(zombieIdleSound);
			audioComponent->Play();
			idleSoundCooldown = 2.0f;
		}
		else
		{
			idleSoundCooldown -= DeltaTime;
			if (idleSoundCooldown < 0.0f)
			{
				idleSoundCooldown = 0.0f;
			}
		}
	}

	if (attackCooldown > 0.0f)
	{
		attackCooldown -= DeltaTime;
		if (attackCooldown <= 0.0f)
		{
			attackCooldown = 0.0f;
			if (is_Attacked)
			{
				is_Attacked = false;
			}
		}
	}
}

void AZombieHorde::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (OtherActor->ActorHasTag(FName("Door")))
		{
			is_StuckByDoor = true;

			targetDoor = Cast<ADoor>(OtherActor);
			status = EZombieStatus::E_ATTACKREADY;

			UE_LOG(LogTemp, Warning, TEXT("Door Overlaped"));
		}
	}
}

void AZombieHorde::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (OtherActor->ActorHasTag(FName("Door")))
		{
			is_StuckByDoor = false;

			targetDoor = nullptr;
			StateChangeToIdle();

			UE_LOG(LogTemp, Warning, TEXT("Door Overlaped End"));
		}
	}
}

void AZombieHorde::MoveFowardToTarget(float DeltaTime)
{
	FVector input = positionToMove - GetActorLocation();
	if (input.Normalize())
	{
		SetActorRotation(FRotationMatrix::MakeFromX(input).Rotator());
		const FVector newLoc = GetActorLocation() + (input * 100.0f * zombieSpeed * DeltaTime);
		SetActorLocation(newLoc);

		if (status != EZombieStatus::E_ATTACKREADY)
		{
			status = EZombieStatus::E_MOVE;
		}
	}
}

void AZombieHorde::SightCheck()
{
	FCollisionQueryParams traceParam;
	traceParam.AddIgnoredActor(this);

	FHitResult hit;

	FVector input = positionToMove - GetActorLocation();
	if (input.Normalize())
	{
		if (GetWorld()->LineTraceSingleByObjectType(OUT hit, GetActorLocation(), GetActorLocation() + (input * 150.0f), FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic), traceParam))
		{
			AActor* actor = hit.GetActor();
			if (actor)
			{
				if (actor->ActorHasTag(FName("Zombie")))
				{
					is_StuckByZombie = true;
				}
			}
		}
		else
		{
			is_StuckByZombie = false;
		}
	}
}

void AZombieHorde::LateInitialize()
{
	for (int i = 0; i < zombieQuantity; i++)
	{
		zombieComponents[i] = NewObject<USkeletalMeshComponent>(this);

		zombieComponents[i]->RegisterComponent();
		zombieComponents[i]->AttachToComponent(rootBoxComponent, FAttachmentTransformRules::KeepWorldTransform, TEXT("Zombie"));

		if (is_Boss)
		{
			zombieComponents[i]->SetSkeletalMesh(bossZombieMesh);
			zombieComponents[i]->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
		}
		else
		{
			int rand = FMath::RandRange(0, 3);
			zombieComponents[i]->SetSkeletalMesh(normalZombieMesh[rand]);
		}
		zombieComponents[i]->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		zombieComponents[i]->SetAnimInstanceClass(zombieAnimInstance);

		zombieComponents[i]->SetRelativeLocationAndRotation(zombiePos[i], FRotator(0.0f, -90.0f, 0.0f));
	}

	if (managerInstance != nullptr)
	{
		managerInstance->SetRemainZombies(managerInstance->GetRemainZombies() + 1);
	}
}

float AZombieHorde::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float finalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	zombieHealth = zombieHealth - finalDamage;

	if (zombieHealth <= 0)
	{
		status = EZombieStatus::E_DEAD;

		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), zombieDeathSound, GetActorLocation());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), zombieDeathParticle, GetActorLocation(), FRotator::ZeroRotator, FVector(5.0f));

		if (managerInstance != nullptr)
		{
			managerInstance->SetRemainZombies(managerInstance->GetRemainZombies() - 1);
		}

		Destroy();
	}

	return finalDamage;
}

void AZombieHorde::StateChangeToIdle()
{
	status = EZombieStatus::E_IDLE;
	idleSoundCooldown = 2.0f;
}
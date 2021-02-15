// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

APlayerCharacter::APlayerCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(TEXT("Player"));

	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	springArm->SetupAttachment(GetCapsuleComponent());
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	camera->SetupAttachment(springArm);
	billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	billboard->SetupAttachment(GetCapsuleComponent());
	billboard->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	springArm->TargetArmLength = 1600.0f;
	springArm->RelativeRotation = FRotator(-30.0f, 0.0f, 0.0f);

	springArm->bUsePawnControlRotation = false;
	springArm->bInheritPitch = false;
	springArm->bInheritRoll = false;
	springArm->bInheritYaw = false;
	springArm->bDoCollisionTest = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> PlayerMesh(
		TEXT("/Game/Model/Player/Meshes/Player.SK_Character_Jock"/*"/Game/Mannequin/Character/Mesh/SK_Mannequin"*/));

	if (PlayerMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(PlayerMesh.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> PlayerAnim(
		TEXT("/Game/Created/PlayerAnimBlueprint"/*"/Game/Mannequin/Animations/ThirdPerson_AnimBP"*/));

	if (PlayerAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(PlayerAnim.Class);
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> iconTexture(
		TEXT("/Game/UI/PlayScene/UpgradeIcon")
	);

	if (iconTexture.Succeeded())
	{
		billboard->SetSprite(iconTexture.Object);
	}
	billboard->SetWorldScale3D(FVector(3.0f, 3.0f, 3.0f));

	is_Carrying = false;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (inputDirection.SizeSquared() > 0.0f)
	{
		GetController()->SetControlRotation(FRotationMatrix::MakeFromX(inputDirection).Rotator());
		AddMovementInput(inputDirection);
	}

	if (is_Carrying)
	{
		billboard->SetHiddenInGame(false);
	}
	else
	{
		billboard->SetHiddenInGame(true);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Interact);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCharacter::MoveRight);
}

void APlayerCharacter::MoveForward(float value)
{
	inputDirection.X = value;
}

void APlayerCharacter::MoveRight(float value)
{
	inputDirection.Y = value;
}

void APlayerCharacter::Interact()
{
	InteractionWithStageObject.Broadcast();
}
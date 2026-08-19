#include "ShowcasePawn.h"


AShowcasePawn::AShowcasePawn()
{
	PrimaryActorTick.bCanEverTick = true;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = false;
	RootComponent = SpringArm;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	const FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, false);
	Camera->AttachToComponent(SpringArm, Rules);
	Camera->SetRelativeLocation(FVector(0));
	// Camera->SetRelativeRotation(FRotator(0, 180, 0));
}

void AShowcasePawn::BeginPlay()
{
	Super::BeginPlay();
	RefreshCamera();
}

void AShowcasePawn::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);
}

void AShowcasePawn::SetupPlayerInputComponent( UInputComponent* PlayerInputComponent )
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/* First click */
	PlayerInputComponent->BindKey(EKeys::A, IE_Pressed, this, &AShowcasePawn::YawLeft);
	PlayerInputComponent->BindKey(EKeys::D, IE_Pressed, this, &AShowcasePawn::YawRight);
	PlayerInputComponent->BindKey(EKeys::W, IE_Pressed, this, &AShowcasePawn::PitchUp);
	PlayerInputComponent->BindKey(EKeys::S, IE_Pressed, this, &AShowcasePawn::PitchDown);
	PlayerInputComponent->BindKey(EKeys::Q, IE_Pressed, this, &AShowcasePawn::ZoomIn);
	PlayerInputComponent->BindKey(EKeys::E, IE_Pressed, this, &AShowcasePawn::ZoomOut);
	/* Repeat */
	PlayerInputComponent->BindKey(EKeys::A, IE_Repeat, this, &AShowcasePawn::YawLeft);
	PlayerInputComponent->BindKey(EKeys::D, IE_Repeat, this, &AShowcasePawn::YawRight);
	PlayerInputComponent->BindKey(EKeys::W, IE_Repeat, this, &AShowcasePawn::PitchUp);
	PlayerInputComponent->BindKey(EKeys::S, IE_Repeat, this, &AShowcasePawn::PitchDown);
	PlayerInputComponent->BindKey(EKeys::Q, IE_Repeat, this, &AShowcasePawn::ZoomIn);
	PlayerInputComponent->BindKey(EKeys::E, IE_Repeat, this, &AShowcasePawn::ZoomOut);
}

void AShowcasePawn::PossessedBy( AController* NewController )
{
	Super::PossessedBy(NewController);

	if ( auto* PC = Cast<APlayerController>(NewController) ) {
		PC->bShowMouseCursor = true;
	}
}

void AShowcasePawn::RefreshCamera()
{
	/* fixing ranges */
	Pitch = FMath::Clamp(Pitch, -80, 10);
	Yaw = FMath::Modulo(Yaw, 360);
	CameraZoom = FMath::Clamp(CameraZoom, 0, 1);

	const auto rot = FRotator(Pitch, Yaw, 0);
	SetActorRotation(rot);

	ensureAlways(CameraCurve);
	const auto CameraDistance = CameraCurve->GetFloatValue(CameraZoom);
	SpringArm->TargetArmLength = CameraDistance;
	// UE_LOGFMT(LogTemp, Verbose, "Camera zoom: {0}", CameraDistance);
}

void AShowcasePawn::YawLeft()
{
	ensureAlways(GetWorld());
	Yaw -= GetWorld()->GetDeltaSeconds() * RotationSpeed;
	RefreshCamera();
}

void AShowcasePawn::YawRight()
{
	ensureAlways(GetWorld());
	Yaw += GetWorld()->GetDeltaSeconds() * RotationSpeed;
	RefreshCamera();
}

void AShowcasePawn::PitchDown()
{
	ensureAlways(GetWorld());
	Pitch += GetWorld()->GetDeltaSeconds() * RotationSpeed;
	RefreshCamera();
}

void AShowcasePawn::PitchUp()
{
	ensureAlways(GetWorld());
	Pitch -= GetWorld()->GetDeltaSeconds() * RotationSpeed;
	RefreshCamera();
}

void AShowcasePawn::ZoomIn()
{
	ensureAlways(GetWorld());
	CameraZoom -= GetWorld()->GetDeltaSeconds() * ZoomSpeed;
	RefreshCamera();
}

void AShowcasePawn::ZoomOut()
{
	ensureAlways(GetWorld());
	CameraZoom += GetWorld()->GetDeltaSeconds() * ZoomSpeed;
	RefreshCamera();
}

bool AShowcasePawn::GetCursorDirection( FVector& Loc, FVector& Dir )
{
	auto* PC = Cast<APlayerController>(GetOwner());
	if ( !PC ) { return false; }

	auto ret = PC->DeprojectMousePositionToWorld(Loc, Dir);
	if ( !ret ) {
		return false;
	}
	return true;
}

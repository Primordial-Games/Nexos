/*////////////////////////////////////////////////////////////////////
* Copyright (c) 2021 Primordial Games LLC
* All rights reserved.
*////////////////////////////////////////////////////////////////////


#include "Player/NexosPlayerCharacterGlobal.h"

#include "Armor/NexosArmorBase.h"

FOnNexosPlayerDrawWeapon ANexosPlayerCharacterGlobal::NotifyWeaponDrawn;
FOnNexosPlayerStowWeapon ANexosPlayerCharacterGlobal::NotifyWeaponStowed;

ANexosPlayerCharacterGlobal::ANexosPlayerCharacterGlobal(const FObjectInitializer& ObjectInitializer)
{
	/*
	 *Setup the First Person meshes
	 */
	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMesh1P"));
	Mesh1P->SetupAttachment(GetCapsuleComponent());
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = true;
	Mesh1P->bCastCinematicShadow = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);

	MeshHelmet1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMeshHelmet1P"));
	MeshHelmet1P->SetupAttachment(GetCapsuleComponent());
	MeshHelmet1P->bOnlyOwnerSee = true;
	MeshHelmet1P->bOwnerNoSee = true;
	MeshHelmet1P->bCastCinematicShadow = false;
	MeshHelmet1P->bReceivesDecals = false;
	MeshHelmet1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	MeshHelmet1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	MeshHelmet1P->SetCollisionObjectType(ECC_Pawn);
	MeshHelmet1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshHelmet1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	MeshArms1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMeshArms1P"));
	MeshArms1P->SetupAttachment(GetCapsuleComponent());
	MeshArms1P->bOnlyOwnerSee = true;
	MeshArms1P->bOwnerNoSee = false;
	MeshArms1P->bCastCinematicShadow = false;
	MeshArms1P->bReceivesDecals = false;
	MeshArms1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	MeshArms1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	MeshArms1P->SetCollisionObjectType(ECC_Pawn);
	MeshArms1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshArms1P->SetCollisionResponseToAllChannels(ECR_Ignore);

	MeshLegs1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMeshLegs1P"));
	MeshLegs1P->SetupAttachment(GetCapsuleComponent());
	MeshLegs1P->bOnlyOwnerSee = true;
	MeshLegs1P->bOwnerNoSee = false;
	MeshLegs1P->bCastCinematicShadow = false;
	MeshLegs1P->bReceivesDecals = false;
	MeshLegs1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	MeshLegs1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	MeshLegs1P->SetCollisionObjectType(ECC_Pawn);
	MeshLegs1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshLegs1P->SetCollisionResponseToAllChannels(ECR_Ignore);

	MeshClassOrnament1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMeshOrnament1P"));
	MeshClassOrnament1P->SetupAttachment(GetCapsuleComponent());
	MeshClassOrnament1P->bOnlyOwnerSee = true;
	MeshClassOrnament1P->bOwnerNoSee = true;
	MeshClassOrnament1P->bCastCinematicShadow = false;
	MeshClassOrnament1P->bReceivesDecals = false;
	MeshClassOrnament1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	MeshClassOrnament1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	MeshClassOrnament1P->SetCollisionObjectType(ECC_Pawn);
	MeshClassOrnament1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshClassOrnament1P->SetCollisionResponseToAllChannels(ECR_Ignore);


	/**
	 *Setup the Third Person Mesh
	 */
	GetMesh()->SetupAttachment(GetCapsuleComponent());
	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	MeshHelmet3P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMeshHelmet3P"));
	MeshHelmet3P->SetupAttachment(GetCapsuleComponent());
	MeshHelmet3P->bOnlyOwnerSee = false;
	MeshHelmet3P->bOwnerNoSee = true;
	MeshHelmet3P->bReceivesDecals = false;
	MeshHelmet3P->SetCollisionObjectType(ECC_Pawn);
	MeshHelmet3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshHelmet3P->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	MeshHelmet3P->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	MeshHelmet3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	MeshArms3P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMeshArms3P"));
	MeshArms3P->SetupAttachment(GetCapsuleComponent());
	MeshArms3P->bOnlyOwnerSee = false;
	MeshArms3P->bOwnerNoSee = true;
	MeshArms3P->bReceivesDecals = false;
	MeshArms3P->SetCollisionObjectType(ECC_Pawn);
	MeshArms3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshArms3P->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	MeshArms3P->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	MeshArms3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	MeshLegs3P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMeshLegs3P"));
	MeshLegs3P->SetupAttachment(GetCapsuleComponent());
	MeshLegs3P->bOnlyOwnerSee = false;
	MeshLegs3P->bOwnerNoSee = true;
	MeshLegs3P->bReceivesDecals = false;
	MeshLegs3P->SetCollisionObjectType(ECC_Pawn);
	MeshLegs3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshLegs3P->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	MeshLegs3P->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	MeshLegs3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	MeshClassOrnament3P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMeshClassOrnament3P"));
	MeshClassOrnament3P->SetupAttachment(GetCapsuleComponent());
	MeshClassOrnament3P->bOnlyOwnerSee = false;
	MeshClassOrnament3P->bOwnerNoSee = true;
	MeshClassOrnament3P->bReceivesDecals = false;
	MeshClassOrnament3P->SetCollisionObjectType(ECC_Pawn);
	MeshClassOrnament3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshClassOrnament3P->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	MeshClassOrnament3P->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	MeshClassOrnament3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);


	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	
    /**
     *Set the Torso as the master Mesh - 1P
     **/
	MeshHelmet1P->SetMasterPoseComponent(Mesh1P);
	MeshArms1P->SetMasterPoseComponent(Mesh1P);
	MeshLegs1P->SetMasterPoseComponent(Mesh1P);
	MeshClassOrnament1P->SetMasterPoseComponent(Mesh1P);
	
	/**
	*Set the Torso as the master Mesh - 3P
	**/
	MeshHelmet3P->SetMasterPoseComponent(GetMesh());
	MeshArms3P->SetMasterPoseComponent(GetMesh());
	MeshLegs3P->SetMasterPoseComponent(GetMesh());
	MeshClassOrnament3P->SetMasterPoseComponent(GetMesh());

	AimingSpeedModifier = 0.5f;
	bIsAiming = false;
	RunSpeedModifier = 1.5f;
	bWantsToRun = false;
	bWantsToFire = false;
	LowHealthPercentage = 0.8f;
	LowSheildsPercentage = 0.7f;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	
	PlayerEquippedWeapons.Add("Primary", nullptr);
	PlayerEquippedWeapons.Add("Special", nullptr);
	PlayerEquippedWeapons.Add("Heavy", nullptr);
	PlayerEquippedWeapons.Add("Sidearm", nullptr);

	PlayerEquippedArmor.Add("Helmet", nullptr);
	PlayerEquippedArmor.Add("Arms", nullptr);
	PlayerEquippedArmor.Add("Torso", nullptr);
	PlayerEquippedArmor.Add("Legs", nullptr);
	PlayerEquippedArmor.Add("Ornament", nullptr);
}

void ANexosPlayerCharacterGlobal::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(GetLocalRole() == ROLE_Authority)
	{
		//Need to get armor and Weapons from Controller
		
	}

	// We want to update the armor
	UpdatePawnMeshes();
	
}

void ANexosPlayerCharacterGlobal::Destroyed()
{
	Super::Destroyed();
	DestroyInventory();
}

void ANexosPlayerCharacterGlobal::PawnClientRestart()
{
	UpdatePawnMeshes();

	SetCurrentWeapon(CurrentWeapon);
	
}

FRotator ANexosPlayerCharacterGlobal::GetAimOffset() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

bool ANexosPlayerCharacterGlobal::IsEnemyFor(AController* TestPC) const
{
	if (TestPC == Controller || TestPC == NULL)
	{
		return false;
	}

	ANexosPlayerState* TestPlayerState = Cast<ANexosPlayerState>(TestPC->PlayerState);
	ANexosPlayerState* MyPlayerState = Cast<ANexosPlayerState>(GetPlayerState());

	bool bIsEnemy = true;
	if (GetWorld()->GetGameState())
	{
		const ANexosColosseumGameModeBase* DefGame = GetWorld()->GetGameState()->GetDefaultGameMode<ANexosColosseumGameModeBase>();
		if (DefGame && MyPlayerState && TestPlayerState)
		{
			bIsEnemy = DefGame->CanDealDamage(TestPlayerState, MyPlayerState);
		}
	}
	return bIsEnemy;
}

//////////////////////////////////////////////////////////////////////////
// Meshes

void ANexosPlayerCharacterGlobal::UpdatePawnMeshes()
{
	/*
	 *Set the Fist Person Meshes
	 */
	if(PlayerEquippedArmor.Find("Torso")->Get() != nullptr)
		Mesh1P->SetSkeletalMesh(PlayerEquippedArmor.Find("Torso")->GetDefaultObject()->ArmorMesh);
	if(PlayerEquippedArmor.Find("Helmet")->Get() != nullptr)
		MeshHelmet1P->SetSkeletalMesh(PlayerEquippedArmor.Find("Helmet")->GetDefaultObject()->ArmorMesh);
	if(PlayerEquippedArmor.Find("Arms")->Get() != nullptr)
		MeshArms1P->SetSkeletalMesh(PlayerEquippedArmor.Find("Arms")->GetDefaultObject()->ArmorMesh);
	if(PlayerEquippedArmor.Find("Legs")->Get() != nullptr)
		MeshLegs1P->SetSkeletalMesh(PlayerEquippedArmor.Find("Legs")->GetDefaultObject()->ArmorMesh);
	if(PlayerEquippedArmor.Find("Ornament")->Get() != nullptr)
		MeshClassOrnament1P->SetSkeletalMesh(PlayerEquippedArmor.Find("Ornament")->GetDefaultObject()->ArmorMesh);

	/*
	 *Set the Third Person Meshes
	 */
	if(PlayerEquippedArmor.Find("Torso")->Get() != nullptr)
		GetMesh()->SetSkeletalMesh(PlayerEquippedArmor.Find("Torso")->GetDefaultObject()->ArmorMesh);
	if(PlayerEquippedArmor.Find("Helmet")->Get() != nullptr)
		MeshHelmet3P->SetSkeletalMesh(PlayerEquippedArmor.Find("Helmet")->GetDefaultObject()->ArmorMesh);
	if(PlayerEquippedArmor.Find("Arms")->Get() != nullptr)
		MeshArms3P->SetSkeletalMesh(PlayerEquippedArmor.Find("Arms")->GetDefaultObject()->ArmorMesh);
	if(PlayerEquippedArmor.Find("Legs")->Get() != nullptr)
		MeshLegs3P->SetSkeletalMesh(PlayerEquippedArmor.Find("Legs")->GetDefaultObject()->ArmorMesh);
	if(PlayerEquippedArmor.Find("Ornament")->Get() != nullptr)
		MeshClassOrnament3P->SetSkeletalMesh(PlayerEquippedArmor.Find("Ornament")->GetDefaultObject()->ArmorMesh);
}

void ANexosPlayerCharacterGlobal::OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation)
{
	/*
	USkeletalMeshComponent* DefMesh1P = Cast<USkeletalMeshComponent>(GetClass()->GetDefaultSubobjectByName(TEXT("PawnMesh1P")));
	const FMatrix DefMeshLS = FRotationTranslationMatrix(DefMesh1P->GetRelativeRotation(), DefMesh1P->GetRelativeLocation());
	const FMatrix LocalToWorld = ActorToWorld().ToMatrixWithScale();

	// Mesh rotating code expect uniform scale in LocalToWorld matrix

	const FRotator RotCameraPitch(CameraRotation.Pitch, 0.0f, 0.0f);
	const FRotator RotCameraYaw(0.0f, CameraRotation.Yaw, 0.0f);

	const FMatrix LeveledCameraLS = FRotationTranslationMatrix(RotCameraYaw, CameraLocation) * LocalToWorld.Inverse();
	const FMatrix PitchedCameraLS = FRotationMatrix(RotCameraPitch) * LeveledCameraLS;
	const FMatrix MeshRelativeToCamera = DefMeshLS * LeveledCameraLS.Inverse();
	const FMatrix PitchedMesh = MeshRelativeToCamera * PitchedCameraLS;

	Mesh1P->SetRelativeLocationAndRotation(PitchedMesh.GetOrigin(), PitchedMesh.Rotator());
	*/
}

//////////////////////////////////////////////////////////////////////////
// Damage & death

void ANexosPlayerCharacterGlobal::FellOutOfWorld(const class UDamageType& dmgType)
{
	Die(999, FDamageEvent(dmgType.GetClass()), NULL, NULL);
}

void ANexosPlayerCharacterGlobal::Suicide()
{
	KilledBy(this);
}

void ANexosPlayerCharacterGlobal::KilledBy(APawn* EventInstigator)
{
}

float ANexosPlayerCharacterGlobal::TakeDamage(float Damage, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
	return 0.f;
}

bool ANexosPlayerCharacterGlobal::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer,
    AActor* DamageCauser) const
{
	return true;
}

bool ANexosPlayerCharacterGlobal::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer,
    AActor* DamageCauser)
{
	return true;
}

void ANexosPlayerCharacterGlobal::OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* InstigatingPawn, AActor* DamageCauser)
{
}

void ANexosPlayerCharacterGlobal::PlayHit(float DamageTaken, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser)
{
	
}

void ANexosPlayerCharacterGlobal::SetRagdollPhysics()
{
	
}

void ANexosPlayerCharacterGlobal::ReplicateHit(float Damage, FDamageEvent const& DamageEvent, APawn* InstigatingPawn, AActor* DamageCauser, bool bKilled)
{
	
}

void ANexosPlayerCharacterGlobal::OnRep_LastTakeHitInfo()
{
	
}

void ANexosPlayerCharacterGlobal::TornOff()
{
	
}

bool ANexosPlayerCharacterGlobal::IsMoving()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Inventory

void ANexosPlayerCharacterGlobal::DestroyInventory()
{
	
}

void ANexosPlayerCharacterGlobal::AddWeaopon(ANexosWeaponBase* Weapon)
{
	
}

void ANexosPlayerCharacterGlobal::RemoveWeapon(ANexosWeaponBase* Weapon)
{
}

ANexosWeaponBase* ANexosPlayerCharacterGlobal::FindWeapon(TSubclassOf<ANexosWeaponBase> WeaponClass)
{
	return CurrentWeapon;
}

void ANexosPlayerCharacterGlobal::DrawWeapon(ANexosWeaponBase* Weapon)
{
	
}

bool ANexosPlayerCharacterGlobal::ServerEquipWeapon_Validate(ANexosWeaponBase* NewWeapon)
{
	return true;
}

void ANexosPlayerCharacterGlobal::ServerEquipWeapon_Implementation(ANexosWeaponBase* NewWeapon)
{
	
}

void ANexosPlayerCharacterGlobal::OnRep_CurrentWeapon(ANexosWeaponBase* LastWeapon)
{
	
}

void ANexosPlayerCharacterGlobal::GetInventoryFromController()
{
	
}


void ANexosPlayerCharacterGlobal::UpdateInventory()
{
}

void ANexosPlayerCharacterGlobal::SetCurrentWeapon(ANexosWeaponBase* NewWeapon, ANexosWeaponBase* LastWeapon)
{
	
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage

void ANexosPlayerCharacterGlobal::StartWeaponFire()
{
	
}

void ANexosPlayerCharacterGlobal::StopWeaponFire()
{
	
}

bool ANexosPlayerCharacterGlobal::CanFire() const
{
	return true;
}

bool ANexosPlayerCharacterGlobal::CanReload() const
{
	return true;
}

void ANexosPlayerCharacterGlobal::SetTarget(bool bNewTargeting)
{
	
}

void ANexosPlayerCharacterGlobal::SetCamera(bool bIsTargetingActive)
{
}

bool ANexosPlayerCharacterGlobal::ServerSetTargeting_Validate(bool bNewTargeting)
{
	return true;
}

void ANexosPlayerCharacterGlobal::ServerSetTargeting_Implementation(bool bNewTargeting)
{
	
}

//////////////////////////////////////////////////////////////////////////
// Abilities



//////////////////////////////////////////////////////////////////////////
// Movement

void ANexosPlayerCharacterGlobal::SetRunning(bool bNewRunning, bool bToggle)
{
	
}

void ANexosPlayerCharacterGlobal::SetJumpAbility(bool bNewJump)
{
}

void ANexosPlayerCharacterGlobal::SetMomentumDrive(bool bNewMomentumDrive)
{
}

bool ANexosPlayerCharacterGlobal::ServerSetRunning_Validate(bool bNewRunning, bool bToggle)
{
	return true;
}

void ANexosPlayerCharacterGlobal::ServerSetRunning_Implementation(bool bNewRunning, bool bToggle)
{
	
}

void ANexosPlayerCharacterGlobal::UpdateRunSounds()
{
	
}

//////////////////////////////////////////////////////////////////////////
// Animations

float ANexosPlayerCharacterGlobal::PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	return 0.f;
}

void ANexosPlayerCharacterGlobal::StopAnimMontage(UAnimMontage* AnimMontage)
{
	
}


void ANexosPlayerCharacterGlobal::StopAllAnimMontages()
{
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANexosPlayerCharacterGlobal::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ANexosPlayerCharacterGlobal::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANexosPlayerCharacterGlobal::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &ANexosPlayerCharacterGlobal::MoveUp);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ANexosPlayerCharacterGlobal::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ANexosPlayerCharacterGlobal::LookUpAtRate);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ANexosPlayerCharacterGlobal::OnStopFire);

	PlayerInputComponent->BindAction("ADS", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnStartAiming);
	PlayerInputComponent->BindAction("ADS", IE_Released, this, &ANexosPlayerCharacterGlobal::OnStopAiming);

	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnNextWeapon);
	PlayerInputComponent->BindAction("PrevWeapon", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnPrevWeapon);

	PlayerInputComponent->BindAction("SwapWeapon", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnSwapWeapons);
	
	PlayerInputComponent->BindAction("PrimaryWeapon", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnPrimaryWeapon);
	PlayerInputComponent->BindAction("SecondaryWeapon", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnSecondaryWeapon);
	PlayerInputComponent->BindAction("HeavyWeapon", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnHeavyWeapon);
	PlayerInputComponent->BindAction("SidearmWeapon", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnSidearmWeapon);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnReload);

	PlayerInputComponent->BindAction("Grenade", IE_Pressed, this, &ANexosPlayerCharacterGlobal::BeginGrenadeThrow);
	PlayerInputComponent->BindAction("Grenade", IE_Released, this, &ANexosPlayerCharacterGlobal::EndGrenadeThrow);

	PlayerInputComponent->BindAction("Melee", IE_Pressed, this, &ANexosPlayerCharacterGlobal::BeginMelee);
	PlayerInputComponent->BindAction("Melee", IE_Released, this, &ANexosPlayerCharacterGlobal::EndMelee);

	PlayerInputComponent->BindAction("Tactical", IE_Pressed, this, &ANexosPlayerCharacterGlobal::BeginTactical);
	PlayerInputComponent->BindAction("Tactical", IE_Released, this, &ANexosPlayerCharacterGlobal::EndTactical);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnStartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ANexosPlayerCharacterGlobal::OnStopJump);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnStartRunning);
	PlayerInputComponent->BindAction("RunToggle", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnStartRunningToggle);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ANexosPlayerCharacterGlobal::OnStopRunning);

	PlayerInputComponent->BindAction("MomentumDrive", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnStartMomentumDrive);
	PlayerInputComponent->BindAction("MomentumDrive", IE_Released, this, &ANexosPlayerCharacterGlobal::OnStopMomentumDrive);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ANexosPlayerCharacterGlobal::OnStartInteract);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ANexosPlayerCharacterGlobal::OnStopInteract);
	
}

void ANexosPlayerCharacterGlobal::MoveForward(float Val)
{
	
}

void ANexosPlayerCharacterGlobal::MoveRight(float Val)
{
	
}

void ANexosPlayerCharacterGlobal::MoveUp(float Val)
{
	
}

void ANexosPlayerCharacterGlobal::TurnAtRate(float Val)
{
	
}

void ANexosPlayerCharacterGlobal::LookUpAtRate(float Val)
{
	
}

void ANexosPlayerCharacterGlobal::OnStartFire()
{
	
}

void ANexosPlayerCharacterGlobal::OnStopFire()
{
	
}

void ANexosPlayerCharacterGlobal::OnStartAiming()
{
	
}

void ANexosPlayerCharacterGlobal::OnStopAiming()
{
	
}

void ANexosPlayerCharacterGlobal::OnNextWeapon()
{
	
}

void ANexosPlayerCharacterGlobal::OnPrevWeapon()
{
	
}

void ANexosPlayerCharacterGlobal::OnSwapWeapons()
{
}

void ANexosPlayerCharacterGlobal::OnPrimaryWeapon()
{
	
}

void ANexosPlayerCharacterGlobal::OnSecondaryWeapon()
{
	
}

void ANexosPlayerCharacterGlobal::OnHeavyWeapon()
{
	
}

void ANexosPlayerCharacterGlobal::OnSidearmWeapon()
{
	
}


void ANexosPlayerCharacterGlobal::OnReload()
{
	
}

void ANexosPlayerCharacterGlobal::OnStartRunning()
{
	
}

void ANexosPlayerCharacterGlobal::OnStartRunningToggle()
{
	
}

void ANexosPlayerCharacterGlobal::OnStopRunning()
{
	
}

void ANexosPlayerCharacterGlobal::BeginGrenadeThrow()
{
}

void ANexosPlayerCharacterGlobal::EndGrenadeThrow()
{
}

void ANexosPlayerCharacterGlobal::BeginMelee()
{
}

void ANexosPlayerCharacterGlobal::EndMelee()
{
}

void ANexosPlayerCharacterGlobal::BeginTactical()
{
}

void ANexosPlayerCharacterGlobal::EndTactical()
{
}

void ANexosPlayerCharacterGlobal::BeginSingularity()
{
}

void ANexosPlayerCharacterGlobal::EndSingularity()
{
}

void ANexosPlayerCharacterGlobal::OnStartMomentumDrive()
{
}

void ANexosPlayerCharacterGlobal::OnStopMomentumDrive()
{
}

void ANexosPlayerCharacterGlobal::OnStartInteract()
{
}

void ANexosPlayerCharacterGlobal::OnStopInteract()
{
}

bool ANexosPlayerCharacterGlobal::IsRunning() const
{
	return true;
}

float ANexosPlayerCharacterGlobal::GetJumpHeightModifier() const
{
	return 0.f;
}

bool ANexosPlayerCharacterGlobal::IsJumping()
{
	return true;
}

void ANexosPlayerCharacterGlobal::Tick(float DeltaSeconds)
{
	
}

void ANexosPlayerCharacterGlobal::BeginDestroy()
{
	Super::BeginDestroy();
}

void ANexosPlayerCharacterGlobal::OnStartJump()
{
	
}

void ANexosPlayerCharacterGlobal::OnStopJump()
{
	
}

//////////////////////////////////////////////////////////////////////////
// Replication

void ANexosPlayerCharacterGlobal::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	
}

void ANexosPlayerCharacterGlobal::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// only to local owner: weapon change requests are locally instigated, other clients don't need it

	// everyone except local owner: flag change is locally instigated
	DOREPLIFETIME_CONDITION(ANexosPlayerCharacterGlobal, bIsAiming, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ANexosPlayerCharacterGlobal, bWantsToRun, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(ANexosPlayerCharacterGlobal, LastTakeHitInfo, COND_Custom);

	// everyone
	DOREPLIFETIME(ANexosPlayerCharacterGlobal, CurrentWeapon);
}

bool ANexosPlayerCharacterGlobal::IsReplicationPausedForConnection(const FNetViewer& ConnectionOwnerNetViewer)
{
	return true;
}

void ANexosPlayerCharacterGlobal::OnReplicationPausedChanged(bool bIsReplicationPaused)
{
	
}

ANexosWeaponBase* ANexosPlayerCharacterGlobal::GetWeapon() const
{
	return CurrentWeapon;
}

ANexosWeaponBase* ANexosPlayerCharacterGlobal::GetLastWeapon() const
{
	return CurrentWeapon;
}

int32 ANexosPlayerCharacterGlobal::GetInventoryCount() const
{
	return 1;
}

ANexosWeaponBase* ANexosPlayerCharacterGlobal::GetInventoryWeapon(int32 Index) const
{
	return CurrentWeapon;
}

USkeletalMeshComponent* ANexosPlayerCharacterGlobal::GetPawnMesh() const
{
	return IsFirstPerson() ? Mesh1P : GetMesh();
}

USkeletalMeshComponent* ANexosPlayerCharacterGlobal::GetSpecifcPawnMesh(bool WantFirstPerson) const
{
	return WantFirstPerson == true ? Mesh1P : GetMesh();
}

FName ANexosPlayerCharacterGlobal::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}

FName ANexosPlayerCharacterGlobal::GetStowedWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}

float ANexosPlayerCharacterGlobal::GetAimingSpeedModifier() const
{
	return AimingSpeedModifier;
}

bool ANexosPlayerCharacterGlobal::IsAiming() const
{
	return bIsAiming;
}

float ANexosPlayerCharacterGlobal::GetRunSpeedModifier() const
{
	return RunSpeedModifier;
}

bool ANexosPlayerCharacterGlobal::IsFiring() const
{
	return bWantsToFire;
}

bool ANexosPlayerCharacterGlobal::IsThrowingGrenade() const
{
	return IsAlive() && Controller && Controller->IsLocalPlayerController();
}

bool ANexosPlayerCharacterGlobal::IsInMelee() const
{
	return false;
}

bool ANexosPlayerCharacterGlobal::IsUsingTactical() const
{
	return false;
}

bool ANexosPlayerCharacterGlobal::IsCastingSingularity() const
{
	return false;
}

bool ANexosPlayerCharacterGlobal::IsFirstPerson() const
{
	return IsAlive() && Controller && Controller->IsLocalPlayerController();
}

bool ANexosPlayerCharacterGlobal::IsAlive() const
{
	return true;
}

void ANexosPlayerCharacterGlobal::BuildPauseReplicationCheckPoints(TArray<FVector>& RelevancyCheckPoints)
{
	FBoxSphereBounds Bounds = GetCapsuleComponent()->CalcBounds(GetCapsuleComponent()->GetComponentTransform());
	FBox BoundingBox = Bounds.GetBox();
	float XDiff = Bounds.BoxExtent.X * 2;
	float YDiff = Bounds.BoxExtent.Y * 2;

	RelevancyCheckPoints.Add(BoundingBox.Min);
	RelevancyCheckPoints.Add(FVector(BoundingBox.Min.X + XDiff, BoundingBox.Min.Y, BoundingBox.Min.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Min.X, BoundingBox.Min.Y + YDiff, BoundingBox.Min.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Min.X + XDiff, BoundingBox.Min.Y + YDiff, BoundingBox.Min.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Max.X - XDiff, BoundingBox.Max.Y, BoundingBox.Max.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Max.X, BoundingBox.Max.Y - YDiff, BoundingBox.Max.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Max.X - XDiff, BoundingBox.Max.Y - YDiff, BoundingBox.Max.Z));
	RelevancyCheckPoints.Add(BoundingBox.Max);
}






















































	
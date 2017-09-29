// Fill out your copyright notice in the Description page of Project Settings.

#include "Personagem.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "PaperFlipbookComponent.h"
#include "Components/ChildActorComponent.h"
#include "Gun.h"
#include "Coin.h"
#include "FinishLine.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"

APersonagem::APersonagem() {
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>
		(TEXT("CameraBoom"));
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>
		(TEXT("Camera"));
	Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
	Camera->OrthoWidth = 2048.0f;
	Camera->SetupAttachment(CameraBoom);
		
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APersonagem::BeginPlay()
{
	Super::BeginPlay();

	if (Idle) {
		GetSprite()->SetFlipbook(Idle);
	}

	if (HUDMobile != NULL) {
		UWorld* World = GetWorld();
		if (World != nullptr) {
			APlayerController* Controller =
				UGameplayStatics::
				GetPlayerController(World, 0);
			if (Controller != nullptr) {
				UUserWidget* UserWidget =
					UWidgetBlueprintLibrary::Create
					(World, HUDMobile, Controller);
				if (UserWidget != nullptr) {
					UserWidget->AddToViewport();
				}
			}
		}
	}
}

void APersonagem::SetupPlayerInputComponent(UInputComponent * 
	PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Move", this, &APersonagem::Move);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this,
		&APersonagem::Jump);
	PlayerInputComponent->BindAction("Fire", IE_Pressed,
		this, &APersonagem::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released,
		this, &APersonagem::StopFire);
	PlayerInputComponent->BindAction("Switch", IE_Pressed,
		this, &APersonagem::SwitchGun);
	PlayerInputComponent->BindAction("Abandon", IE_Pressed,
		this, &APersonagem::AbandonGun);

	PlayerInputComponent->BindTouch(IE_Pressed, this,
		&APersonagem::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this,
		&APersonagem::TouchStopped);

}

void APersonagem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateFlipbook();

}

void APersonagem::UpdateFlipbook()
{

	if (GetVelocity().X != 0) {
		GetSprite()->SetFlipbook(Walking);
		if (GetVelocity().X > 0) {
			GetSprite()->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));
		} else if (GetVelocity().X < 0) {
			GetSprite()->SetWorldRotation(FRotator(0.0f, 180.0f, 0.0f));
		}
	} else {
		GetSprite()->SetFlipbook(Idle);
	}

}

void APersonagem::StartFire()
{

	if (Guns.Num() > 0 && SelectedGun >= 0 && Guns[SelectedGun]!=nullptr) {
		Guns[SelectedGun]->StartFire();
	}

}

void APersonagem::StopFire()
{

	if (Guns.Num() > 0 && SelectedGun >= 0 && Guns[SelectedGun] != nullptr) {
		Guns[SelectedGun]->StopFire();
	}

}

void APersonagem::AddGunToArray(AGun * Gun)
{
	Gun->SetActorRelativeRotation(GetActorRotation());
	Gun->GetCollisionComp()->SetCollisionProfileName("NoCollision");
	Guns.Add(Gun);
	FAttachmentTransformRules TransformRules(EAttachmentRule::KeepWorld, true);
	Gun->AttachToComponent(GetSprite(), TransformRules);
	Gun->SetActorRelativeLocation(FVector(45.0f, 0.0f, 10.0f));
	int LastSelected = SelectedGun;
	SelectedGun = Guns.Num()-1;
	if (LastSelected >= 0) {
		Guns[LastSelected]->SetActorHiddenInGame(true);
	}
}

int APersonagem::GetSelectedGun()
{
	return SelectedGun;
}

TArray<class AGun*> APersonagem::GetGuns()
{
	return Guns;
}

void APersonagem::Move(float Value)
{
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void APersonagem::TouchStarted(const ETouchIndex::Type FinderIndex, const FVector Location)
{

	Jump();

}

void APersonagem::TouchStopped(const ETouchIndex::Type FinderIndex, const FVector Location)
{

	StopJumping();

}

void APersonagem::SwitchGun()
{

	if (SelectedGun == Guns.Num() - 1) {
		SelectedGun = -1;
	} else {
		SelectedGun++;
	}

	if (SelectedGun >= 0) {
		Guns[SelectedGun]->SetActorHiddenInGame(false);
		if (SelectedGun > 0) {
			Guns[SelectedGun - 1]->SetActorHiddenInGame(true);
		}
	} else if (Guns.Num() > 0) {
		Guns[Guns.Num() - 1]->SetActorHiddenInGame(true);
	}

}


void APersonagem::AbandonGun()
{
	if (Guns[SelectedGun] != nullptr) {
		Guns[SelectedGun]->Destroy();
		UE_LOG(LogTemp, Warning, TEXT("ABANDON GUN"));
	}


}

void APersonagem::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

	if (OtherActor != nullptr && OtherActor->IsA(AGun::StaticClass())) {
		SwitchGun();
		SelectedGun++;
	}
	if (OtherActor != nullptr && OtherActor->IsA(ACoin::StaticClass())) {
		Moedas++;
		UE_LOG(LogTemp, Warning, TEXT("VOCE PEGOU %s MOEDAS"), Moedas);
	}

}

//void APersonagem::CollectedAll()
//{
//	if (Moedas >= 5) {
//		UWorld* World = GetWorld();
//		if (World != nullptr) {
//			FActorSpawnParameters SpawnParam;
//			AFinishLine* FinishLine = World->SpawnActor<AFinishLine>(FinishLineBP, 4660, 343, SpawnParam);
//			UE_LOG(LogTemp, Warning, TEXT("LINHA DE CHEGADA APARECEU"));
//		}
//	}

//}
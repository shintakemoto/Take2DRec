// Fill out your copyright notice in the Description page of Project Settings.

#include "Bomb.h"
#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperFlipbookComponent.h"
#include "Personagem.h"


// Sets default values
ABomb::ABomb()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	CollisionComp->bGenerateOverlapEvents = true;
	CollisionComp->SetCollisionProfileName("OverlapAllDynamic");
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABomb::OnOverlapBegin);
	RootComponent = CollisionComp;

	Sprite = CreateDefaultSubobject
		<UPaperSpriteComponent>(TEXT("Sprite"));
	Sprite->SetCollisionProfileName("NoCollision");
	Sprite->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABomb::OnOverlapBegin(UPrimitiveComponent * OverlappedComp,
	AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

	if (OtherActor != nullptr &&
		OtherActor->IsA(APersonagem::StaticClass())) {
		UE_LOG(LogTemp, Warning, TEXT("VOCE PERDEU UMA VIDA"));
		Destroy();

	}



}


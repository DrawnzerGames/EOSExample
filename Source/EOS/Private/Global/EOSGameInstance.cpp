// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/EOSGameInstance.h"
#include "Manager/LoginManager.h"

void UEOSGameInstance::Init()
{
    Super::Init();

    //Create Singleton instances during init of GameInstance, this ensures thread safety of Singleton objects.
    ULoginManager::CreateInstance();
    ULoginManager::Get()->InitManager();
    LoginProvider = NewObject<ULoginProvider>(GetTransientPackage(), TEXT("MY_LOGIN_PROVIDER"));
    ULoginManager::Get()->SetLoginProvider(LoginProvider);
}
// Fill out your copyright notice in the Description page of Project Settings.

#include "EOS/Public/Provider/LoginProvider.h"
#include "EOS/Private/Manager/LoginManager.h"

ULoginProvider::ULoginProvider()
{
}

void ULoginProvider::Login(int LocalUserNum, FString LoginType)
{
    ULoginManager::Get()->Login(LocalUserNum, LoginType);
}

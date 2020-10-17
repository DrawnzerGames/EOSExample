// Copyright 2020 - Infinity DrawnzerGames, Inc. All Rights Reserved.

#include "EOS/Public/Provider/LoginProvider.h"
#include "EOS/Private/Manager/LoginManager.h"

ULoginProvider::ULoginProvider()
{
}

void ULoginProvider::Login(int LocalUserNum, FString LoginType)
{
    ULoginManager::Get()->Login(LocalUserNum, LoginType);
}

// Copyright 2020 - Infinity DrawnzerGames, Inc. All Rights Reserved.

#include "LoginManager.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

ULoginManager* ULoginManager::LoginManager;

void ULoginManager::CreateInstance()
{
    LoginManager = NewObject<ULoginManager>(GetTransientPackage(), NAME_None);
    LoginManager->AddToRoot();
}

void ULoginManager::InitManager()
{
    //Init Manager code here.
}

ULoginManager* ULoginManager::Get()
{
    return LoginManager;
}

void ULoginManager::SetLoginProvider(ULoginProvider* InLoginProvider)
{
    LoginProvider = InLoginProvider;
}

void ULoginManager::Login(int LocalUserNum, FString LoginType)
{
    const auto Identity = IOnlineSubsystem::Get("EOS")->GetIdentityInterface().Get();
    if (Identity)
    {
        const TWeakObjectPtr<ULoginManager> WeakThis(this);
        Identity->OnLoginCompleteDelegates->AddLambda([WeakThis, Identity](int LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
        {
            if (WeakThis.Get())
            {
                if (bWasSuccessful)
                {
                    if (WeakThis.Get()->LoginProvider.Get())
                    {
                        FString Message = "Login Complete Without Errors, User id = ";
                        Message.Append(Identity->GetUniquePlayerId(LocalUserNum).Get()->ToString());
                        WeakThis.Get()->LoginProvider->OnLoginCompleteCallback.Broadcast(true, Message);
                    }
                }
                else if (bWasSuccessful == false && Error == "Already logged in")
                {
                    if (WeakThis.Get()->LoginProvider.Get())
                    {
                        FString Message = "Already logged in, User id = ";
                        Message.Append(Identity->GetUniquePlayerId(LocalUserNum).Get()->ToString());
                        WeakThis.Get()->LoginProvider->OnLoginCompleteCallback.Broadcast(true, Message);
                    }
                }
                else
                {
                    //Todo -> Handle error conditions more generously.
                    WeakThis.Get()->LoginProvider->OnLoginCompleteCallback.Broadcast(false, Error);
                }
                Identity->OnLoginCompleteDelegates->RemoveAll(WeakThis.Get());
                Identity->OnLoginCompleteDelegates->Clear();
            }
        });

        //My machine specific credentials, please change as per yours.
        FOnlineAccountCredentials AccountCredentials;
        AccountCredentials.Type = LoginType;
        AccountCredentials.Id = TEXT("localhost:12345");
        AccountCredentials.Token = TEXT("TEST_USER_1");
        if (LoginType == TEXT("weblogin") || LoginType == TEXT("persistweblogin"))
        {
            AccountCredentials.Id = TEXT("ACCOUNT_ID");
            AccountCredentials.Token = TEXT("ACCOUNT_TOKEN");
        }
        Identity->Login(LocalUserNum, AccountCredentials);
    }
}

ULoginManager::ULoginManager()
{
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EOS/Public/Provider/LoginProvider.h"
#include "EOS/Public/Provider/Interface/LoginInterface.h"
#include "UObject/NoExportTypes.h"
#include "LoginManager.generated.h"

/**
 * 
 */
UCLASS()
class ULoginManager : public UObject, public ILoginInterface
{
    GENERATED_BODY()

public:
    static void CreateInstance();
    void InitManager();
    static ULoginManager* Get();
    void SetLoginProvider(ULoginProvider* InLoginProvider);

    //ILoginInterface overrides.
    virtual void Login(int LocalUserNum, FString LoginType) override;

private:
    static ULoginManager* LoginManager;
    ULoginManager();

    TWeakObjectPtr<ULoginProvider> LoginProvider;
};

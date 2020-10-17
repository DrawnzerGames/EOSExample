// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/LoginInterface.h"
#include "LoginProvider.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginCompleteCallback, bool, bResult, FString, Message);

/**
 * 
 */
UCLASS()
class EOS_API ULoginProvider : public UObject, public ILoginInterface
{
    GENERATED_BODY()

public:
    ULoginProvider();

    UPROPERTY(BlueprintReadOnly, BlueprintAssignable, Category=EOS)
    FOnLoginCompleteCallback OnLoginCompleteCallback;

    //ILoginInterface overrides.
    UFUNCTION(BlueprintCallable)
    virtual void Login(int LocalUserNum, FString LoginType) override;
};

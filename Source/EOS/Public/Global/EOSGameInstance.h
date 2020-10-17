// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Provider/LoginProvider.h"
#include "EOSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class EOS_API UEOSGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    //Make blueprint accessible to call and get callbacks.
    UPROPERTY(BlueprintReadOnly, Category=EOSLogin)
    ULoginProvider* LoginProvider;
};

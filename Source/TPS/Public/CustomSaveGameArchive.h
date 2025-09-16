// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

/**
 * 
 */
struct FCustomSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
    FCustomSaveGameArchive(FArchive& InInnerArchive)
        : FObjectAndNameAsStringProxyArchive(InInnerArchive, false)
    {
        ArIsSaveGame = true;
        ArNoDelta = true;
    }
};
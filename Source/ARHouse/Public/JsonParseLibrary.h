// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonParseLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ARHOUSE_API UJsonParseLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FString JsonParse(const FString& originData);

	// 지금은 제이슨 형태로 스트링으로 받으려고 스트링 형태로 함수 만듦
	static FString MakeJson(const TMap<FString, FString> source);

	
};

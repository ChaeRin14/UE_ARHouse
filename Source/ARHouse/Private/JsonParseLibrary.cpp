// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonParseLibrary.h"
#include "Json.h"

FString UJsonParseLibrary::JsonParse(const FString& originData)
{
	FString parsedData;

	// Reader를 만든다 읽어오려고
	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(originData);

	//  Reader로 파싱된 결과를 담을 json Object를 선언한다
	TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());

	// 해독한다

	if (FJsonSerializer::Deserialize(reader, result))
	{
		// items라는 배열에서
		TArray<TSharedPtr<FJsonValue>> parsedDataArr = result->GetArrayField("items");

		// 그 안에 있는 내용에서 bookName과 authorName만 알고 싶어서
		for (TSharedPtr<FJsonValue> pd : parsedDataArr)
		{
			FString bookName = pd->AsObject()->GetStringField("bk_nm");
			FString authorName = pd->AsObject()->GetStringField("aut_nm");

			// 두 개의 값을 최종 반환할 문자열 데이터에 추가해 주기
			parsedData.Append(FString::Printf(TEXT("Book Name : %s\nAuthor Name : %s\n\n"), *bookName, *authorName));
		}
	}

	return parsedData;
}

FString UJsonParseLibrary::MakeJson(const TMap<FString, FString> source)
{
	// json object를 생성하고 값을 넣는다
	TSharedPtr<FJsonObject> jsonObj = MakeShareable(new FJsonObject());

	for (TPair<FString, FString> kv : source)
	{
		jsonObj->SetStringField(kv.Key, kv.Value);
	}

	// Writer를 생성하고, json object를 인코딩한다
	FString jsonData;

	TSharedRef<TJsonWriter<TCHAR>> writer = TJsonWriterFactory<TCHAR>::Create(&jsonData);
	FJsonSerializer::Serialize(jsonObj.ToSharedRef(), writer);

	return jsonData;

}


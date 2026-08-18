#pragma once


#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"


#include "SQLiteDatabaseConnection.h"


#include "SqlParser.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(SQLParser, Verbose, Verbose);


USTRUCT(BlueprintType)
struct FInvestment {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 id = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 buildings = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString city;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString address;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int active = 0;
};


UCLASS(BlueprintType)
class USQL_Parser : public UGameInstanceSubsystem {
	GENERATED_BODY()


public:
	virtual void Initialize( FSubsystemCollectionBase& Collection ) override;

	virtual void Deinitialize() override;


protected:
	/* Open specific base */
	UFUNCTION(BlueprintCallable)
	bool OpenDatabase();

	bool OpenDatabase( const FString& DatabasePath );

	void CloseDatabase();

	// bool Execute( const FString& Query );

	bool MakeQuery( const FString& Query, FDataBaseRecordSet*& Results ) const;


public:
	/* Get list of all investments */
	UFUNCTION(BlueprintCallable)
	void GetInvestments( TArray<FInvestment>& Result ) const;


private:
	TUniquePtr<FSQLiteDatabaseConnection> Database = nullptr;
};

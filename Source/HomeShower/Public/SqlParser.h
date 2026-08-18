#pragma once


#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"


#include "SQLiteDatabaseConnection.h"


#include "SqlParser.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(SQLParser, Verbose, Verbose);


USTRUCT(BlueprintType)
struct FInvestments {
	GENERATED_BODY()
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

	bool MakeQuery( const FString& Query, FSQLiteResultSet*& Results );


public:
	/* Get list of all investments */
	UFUNCTION(BlueprintCallable)
	void GetInvestments( TArray<FInvestments>& Result );


private:
	TUniquePtr<FSQLiteDatabaseConnection> Database = nullptr;
};

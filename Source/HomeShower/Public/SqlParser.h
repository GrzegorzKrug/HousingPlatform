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


UENUM(BlueprintType)
enum class EFlatStatus: uint8 {
	Hidden    = 0,
	Available = 1,
	Reserved  = 2,
	Sold      = 3
};


UENUM(BlueprintType, meta=(Bitflags))
enum class EFlatExtras: uint8 {
	None   = 0,
	Garden = 1 << 0,
	Balcon = 1 << 1,
	Garage = 1 << 2
};


USTRUCT(BlueprintType)
struct FFlat {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int id = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int buildingId = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int rooms = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int price = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EFlatStatus Status = EFlatStatus::Hidden;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int active = -1;
};


USTRUCT(BlueprintType)
struct FFloor {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int FloorI = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<int, FFlat> Flats;
};


USTRUCT(BlueprintType)
struct FBuilding {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int id = -1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<int, FFloor> Floors;

	/* Median price in builidng */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MedPricePerSqm = 0;
	int priceCounter = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int FlatCounter = 0;
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
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	void GetInvestments( TArray<FInvestment>& Result ) const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	void GetBuildings( int InvestmentId, TArray<FBuilding>& OutBuildings ) const;


private :
	TUniquePtr<FSQLiteDatabaseConnection> Database = nullptr;
};

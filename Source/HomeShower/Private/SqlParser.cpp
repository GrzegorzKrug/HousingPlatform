#include "SqlParser.h"


#include "SQLiteDatabaseConnection.h"
#include "SQLiteResultSet.h"


DEFINE_LOG_CATEGORY(SQLParser)


void USQL_Parser::Initialize( FSubsystemCollectionBase& Collection )
{
	Super::Initialize(Collection);
	OpenDatabase();
}

void USQL_Parser::Deinitialize()
{
	CloseDatabase();
	Super::Deinitialize();
}

bool USQL_Parser::OpenDatabase()
{
	if ( Database ) {
		UE_LOGFMT(SQLParser, Warning, "Database was open. Keeping current one");
		return true;
	}

	const auto Path = FPaths::Combine(FPaths::ProjectDir(), FString("Database/odyssey_demo.db"));
	return OpenDatabase(Path);
}


bool USQL_Parser::OpenDatabase( const FString& DatabasePath )
{
	if ( Database ) {
		UE_LOGFMT(SQLParser, Warning, "SQLParser: Opening new database without closing other");
		CloseDatabase();
	}
	UE_LOGFMT(SQLParser, Log, "Opening database with path: {0}", DatabasePath);

	ensureAlways(!Database);
	Database = MakeUnique<FSQLiteDatabaseConnection>();

	auto ret = Database->Open(*DatabasePath, {}, {});

	if ( ret && Database.IsValid() ) {
		UE_LOGFMT(SQLParser, Log, "Opening successful");
	}
	else {
		UE_LOGFMT(SQLParser, Warning, "Opening failed");
		Database->Close();
	}
	return ret;
}

void USQL_Parser::CloseDatabase()
{
	if ( Database ) {
		UE_LOGFMT(SQLParser, Log, "Closing database");
		Database->Close();
	}
	Database = nullptr;
}

// bool USQL_Parser::Execute( const FString& Query )
// {
// 	if ( !Database ) {
// 		return false;
// 	}
// 	UE_LOGFMT(SQLParser, Log, "Executing query");
// 	return {};
// }

bool USQL_Parser::MakeQuery( const FString& Query, FDataBaseRecordSet*& Results ) const
{
	if ( !Database ) {
		return false;
	}


	if ( !Database->Execute(*Query, Results) ) {
		return false;
	}

	return true;
}

void USQL_Parser::GetInvestments( TArray<FInvestment>& Result ) const
{
	if ( !Database ) {
		UE_LOGFMT(SQLParser, Warning, "Get Investment can not work on closed DB");
		return;
	}

	const FString Query = TEXT(
		R"(
		SELECT
			i.id,
			i.name,
			i.city,
			i.address,
			i.description,
			i.currency,
			i.active,
			COUNT(b.id) AS buildings_count
		FROM investments i
		LEFT JOIN buildings b
			ON b.investment_id = i.id
			AND b.active = 1
		WHERE i.active = 1
		GROUP BY i.id
		ORDER BY i.id;
	)"
	);

	FDataBaseRecordSet* Res = nullptr;
	MakeQuery(Query, Res);

	if ( Res ) {
		UE_LOGFMT(SQLParser, Log, "Query was ok");
	}
	else {
		UE_LOGFMT(SQLParser, Log, "Wrong query or something");
		return;
	}

	Result.Empty();
	Result.Reserve(Res->GetRecordCount());
	for ( FDataBaseRecordSet::TIterator It(Res); It; ++It ) {
		/**/
		FInvestment inv;
		inv.id = It->GetInt(TEXT("id"));
		inv.active = It->GetInt(TEXT("active"));
		inv.buildings = It->GetInt(TEXT("buildings_count"));

		inv.name = It->GetString(TEXT("name"));
		inv.city = It->GetString(TEXT("city"));
		inv.address = It->GetString(TEXT("address"));

		if ( inv.active <= 0 ) {
			UE_LOGFMT(SQLParser, Error, "Ignoring invalid investment that is invalid");
			continue;
		}

		Result.Add(inv);
	}


	/* Deallocation or nothing */
	delete Res;
}

void USQL_Parser::GetBuildings( int InvestmentId, TArray<FBuilding>& OutBuildings ) const
{
	OutBuildings.Empty();

	if ( !Database ) {
		UE_LOGFMT(SQLParser, Warning, "GetBuildings cannot work on closed DB");
		return;
	}


	UE_LOGFMT(SQLParser, Log, "Query made for buildings in {0} investment", InvestmentId);
	const FString Query = FString::Printf(
		TEXT(
			R"(
		SELECT
			b.id AS building_id,
			b.name AS building_name,
			b.code AS building_code,
			b.floors_count,
			b.order_number,

			f.id AS flat_id,
			f.name AS flat_name,
			f.status_id as status_code,
			f.entry_floor,
			f.floor,
			f.num_rooms,
			f.area,
			f.price,
			f.price_sqm,
			f.mesh_id,
			f.description AS flat_description

		FROM buildings b

		LEFT JOIN flats f
			ON f.building_id = b.id
			AND f.active = 1

		LEFT JOIN flat_statuses s
			ON s.id = f.status_id

		WHERE b.investment_id = %d
			AND b.active = 1

		ORDER BY
			b.order_number,
			b.id,
			f.floor,
			f.id;
	)"
		),
		InvestmentId
	);

	FDataBaseRecordSet* Res = nullptr;
	MakeQuery(Query, Res);

	if ( !Res ) {
		UE_LOGFMT(SQLParser, Warning, "Failed to get buildings for investment {InvestmentId}", InvestmentId);
		return;
	}

	// int64 CurrentBuildingId = -1;
	// int32 CurrentFloorNumber = -1;

	TMap<int, FBuilding> BudMap;
	// TSet<FFloor> FloorSet;

	for ( FDataBaseRecordSet::TIterator It(Res); It; ++It ) {
		FBuilding CheckBud{};
		CheckBud.id = It->GetInt(TEXT("building_id"));
		CheckBud.name = It->GetString(TEXT("building_name"));
		// ThisBuilding-> = It->GetInt(TEXT("floors_count"));
		// NewBuilding.OrderNumber = It->GetInt(TEXT("order_number"));

		if ( !BudMap.Contains(CheckBud.id) ) {
			BudMap.Add(CheckBud.id, CheckBud);
		}
		FBuilding& ThisBuilding = BudMap[CheckBud.id];

		FFloor CheckFloor{};
		// CheckFloor.FloorI = 1; /* miss understood */
		CheckFloor.FloorI = It->GetInt(TEXT("floor"));

		auto& Fl = ThisBuilding.Floors;
		if ( !Fl.Contains(CheckFloor.FloorI) ) {
			Fl.Add(CheckFloor.FloorI, CheckFloor);
		}
		FFloor& CurrentFloor = Fl[CheckFloor.FloorI];

		FFlat CheckFlat{};
		CheckFlat.id = It->GetInt(TEXT("flat_id"));
		CheckFlat.Status = static_cast<EFlatStatus>(It->GetInt(TEXT("status_code")));
		// auto Status = It->GetString(TEXT("status_code"));
		CheckFlat.price = It->GetInt(TEXT("price"));
		CheckFlat.rooms = It->GetInt(TEXT("num_rooms"));
		ensureAlways(!CurrentFloor.Flats.Contains(CheckFlat.id));

		if ( CheckFlat.price > 0 ) {
			ThisBuilding.MedPricePerSqm += CheckFlat.price;
			ThisBuilding.priceCounter += 1;
		}

		/* Non shipping checks */
		const auto val = static_cast<int>(CheckFlat.Status); /* 1-3 Mapped in dB */
		ensureAlways(val>=1 && val<=3);

		CurrentFloor.Flats.Add(CheckFlat.id, CheckFlat);
		ThisBuilding.FlatCounter += 1;
	}

	for ( auto& pair : BudMap ) {
		UE_LOGFMT(
			SQLParser,
			Log,
			"Parsing zone: {0}, bud: {1}, has {2} flats. MedPrice: {3}",
			InvestmentId,
			pair.Value.id,
			pair.Value.FlatCounter,
			pair.Value.MedPricePerSqm
		);
		pair.Value.MedPricePerSqm /= pair.Value.priceCounter;
		OutBuildings.Add(pair.Value);
	}

	delete Res;
}

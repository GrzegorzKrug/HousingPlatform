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

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

bool USQL_Parser::MakeQuery( const FString& Query, FSQLiteResultSet*& Results )
{
	if ( !Database ) {
		return false;
	}


	if ( !Database->Execute(*Query, Results) ) {
		return false;
	}

	return true;
}

void USQL_Parser::GetInvestments( TArray<FInvestments>& )
{
	if ( !Database ) {
		UE_LOGFMT(SQLParser, Warning, "Get Investment can not work on closed DB");
		return;
	}

	const FString Query = TEXT(
		R"(
		SELECT
			id,
			name,
			city,
			address,
			description,
			currency
		FROM investments
		WHERE active = 1
		ORDER BY id;
	)"
	);
	
	FSQLiteResultSet* res = nullptr;
	MakeQuery(Query, res);

	if ( res ) {
		UE_LOGFMT(SQLParser, Log, "Query was ok");
	}
	else {
		UE_LOGFMT(SQLParser, Log, "Wrong query or something");
	}

	/* Deallocation or nothing */
	delete res;
}

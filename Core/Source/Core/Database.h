#pragma once
#include "sqlite3.h"
#include <string>
#include <vector>

namespace Core {
	class Database {
	public:
		Database(const std::string& dbName);
		~Database();

		bool createTables();
		bool addNote(const std::string& title, const std::string& content, const std::vector<std::string>& tags, const std::string& creationDate);

	private:
		sqlite3* db;
		std::string dbName;
	};
}
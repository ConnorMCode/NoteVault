#pragma once
#include "sqlite3.h"
#include <string>
#include <vector>
#include <optional>
#include "Note.h"

namespace Core {
	class Database {
	public:
		Database(const std::string& dbName);
		~Database();

		bool createTables();
		bool addNote(int id, const std::string& title, const std::string& content, const std::vector<std::string>& tags, const std::string& creationDate);

		std::optional<Note> getNoteByTitle(const std::string& title);

		std::optional<Note> getRandomNote();

		std::vector<Note> getAllNotes();

	private:
		sqlite3* db;
		std::string dbName;
	};
}
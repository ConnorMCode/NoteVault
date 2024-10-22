#include "Database.h"
#include <iostream>

namespace Core {
    Database::Database(const std::string& dbName) : dbName(dbName), db(nullptr) {
        if (sqlite3_open(dbName.c_str(), &db)) {
            std::cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << std::endl;
        }
        else {
            std::cout << "Database opened successfully!" << std::endl;
        }
    }

    Database::~Database() {
        if (db) {
            sqlite3_close(db);
        }
    }

    bool Database::createTables(){
        // SQL to create the Notes table
        std::string sqlNotes = "CREATE TABLE IF NOT EXISTS Notes ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "Title TEXT NOT NULL, "
            "Content TEXT NOT NULL, "
            "CreationDate TEXT NOT NULL);";

        // SQL to create the Tags table
        std::string sqlTags = "CREATE TABLE IF NOT EXISTS Tags ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "Name TEXT NOT NULL UNIQUE);";

        // SQL to create the NoteTags table (many-to-many relationship)
        std::string sqlNoteTags = "CREATE TABLE IF NOT EXISTS NoteTags ("
            "NoteID INTEGER, "
            "TagID INTEGER, "
            "FOREIGN KEY (NoteID) REFERENCES Notes(ID) ON DELETE CASCADE, "
            "FOREIGN KEY (TagID) REFERENCES Tags(ID) ON DELETE CASCADE, "
            "PRIMARY KEY (NoteID, TagID));";

        char* errMsg = nullptr;

        // Execute creation of Notes table
        int rc = sqlite3_exec(db, sqlNotes.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error creating Notes table: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }

        // Execute creation of Tags table
        rc = sqlite3_exec(db, sqlTags.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error creating Tags table: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }

        // Execute creation of NoteTags table
        rc = sqlite3_exec(db, sqlNoteTags.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error creating NoteTags table: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }

        std::cout << "Tables created successfully!" << std::endl;
        return true;
    }


    bool Database::addNote(const std::string& title, const std::string& content, const std::vector<std::string>& tags, const std::string& creationDate) {
        // Start a transaction to ensure atomicity
        sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

        // Insert the note into the Notes table
        std::string sqlNote = "INSERT INTO Notes (Title, Content, CreationDate) VALUES (?, ?, ?);";
        sqlite3_stmt* stmtNote;

        int rc = sqlite3_prepare_v2(db, sqlNote.c_str(), -1, &stmtNote, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare note insert statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_text(stmtNote, 1, title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtNote, 2, content.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtNote, 3, creationDate.c_str(), -1, SQLITE_STATIC);

        // Execute note insertion
        rc = sqlite3_step(stmtNote);
        if (rc != SQLITE_DONE) {
            std::cerr << "Note insertion failed: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmtNote);
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr); // Rollback transaction on error
            return false;
        }

        // Get the inserted note's ID
        int noteID = sqlite3_last_insert_rowid(db);
        sqlite3_finalize(stmtNote);

        // Insert each tag into the Tags table (if it doesn't already exist) and insert into NoteTags table
        std::string sqlTag = "INSERT OR IGNORE INTO Tags (Name) VALUES (?);";
        std::string sqlNoteTag = "INSERT INTO NoteTags (NoteID, TagID) VALUES (?, ?);";
        sqlite3_stmt* stmtTag;
        sqlite3_stmt* stmtNoteTag;

        for (const std::string& tag : tags) {
            // Prepare tag insertion
            rc = sqlite3_prepare_v2(db, sqlTag.c_str(), -1, &stmtTag, nullptr);
            if (rc != SQLITE_OK) {
                std::cerr << "Failed to prepare tag insert statement: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr); // Rollback transaction
                return false;
            }

            // Bind tag name and execute
            sqlite3_bind_text(stmtTag, 1, tag.c_str(), -1, SQLITE_STATIC);
            rc = sqlite3_step(stmtTag);
            if (rc != SQLITE_DONE && rc != SQLITE_CONSTRAINT) {  // Ignore unique constraint error
                std::cerr << "Tag insertion failed: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmtTag);
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr); // Rollback transaction
                return false;
            }
            sqlite3_finalize(stmtTag);

            // Get the tag ID (either inserted or existing)
            std::string sqlGetTagID = "SELECT ID FROM Tags WHERE Name = ?;";
            sqlite3_prepare_v2(db, sqlGetTagID.c_str(), -1, &stmtTag, nullptr);
            sqlite3_bind_text(stmtTag, 1, tag.c_str(), -1, SQLITE_STATIC);
            rc = sqlite3_step(stmtTag);
            int tagID = sqlite3_column_int(stmtTag, 0);
            sqlite3_finalize(stmtTag);

            // Prepare NoteTags insertion
            rc = sqlite3_prepare_v2(db, sqlNoteTag.c_str(), -1, &stmtNoteTag, nullptr);
            if (rc != SQLITE_OK) {
                std::cerr << "Failed to prepare NoteTags insert statement: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr); // Rollback transaction
                return false;
            }

            // Bind noteID and tagID
            sqlite3_bind_int(stmtNoteTag, 1, noteID);
            sqlite3_bind_int(stmtNoteTag, 2, tagID);

            // Execute NoteTags insertion
            rc = sqlite3_step(stmtNoteTag);
            if (rc != SQLITE_DONE) {
                std::cerr << "NoteTags insertion failed: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmtNoteTag);
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr); // Rollback transaction
                return false;
            }

            sqlite3_finalize(stmtNoteTag);
        }

        // Commit the transaction
        sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
        std::cout << "Note and tags added successfully!" << std::endl;

        return true;
    }

    std::optional<Note> Database::getNoteByTitle(const std::string& title) {
        std::string sql = "SELECT ID, Title, Content, CreationDate FROM Notes WHERE Title = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Error preparing statement to retrieve note by title: " << sqlite3_errmsg(db) << std::endl;
            return std::nullopt;
        }

        sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string noteTitle = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string creationDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

            sqlite3_finalize(stmt);
            return Note{ noteTitle, content, {}, creationDate };
        }
        else {
            std::cerr << "Note not found with title: " << title << std::endl;
        }

        sqlite3_finalize(stmt);
        return std::nullopt;
    }

    std::optional<Note> Database::getRandomNote() {
        std::string sql = "SELECT ID, Title, Content, CreationDate FROM Notes ORDER BY RANDOM() LIMIT 1;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Error preparing statement to retrieve random note: " << sqlite3_errmsg(db) << std::endl;
            return std::nullopt;
        }

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string creationDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

            sqlite3_finalize(stmt);
            return Note{ title, content, {}, creationDate };
        }
        else {
            std::cerr << "No note found in the database." << std::endl;
        }

        sqlite3_finalize(stmt);
        return std::nullopt;
    }

}
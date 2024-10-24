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


    bool Database::addNote(int id, const std::string& title, const std::string& content, const std::vector<std::string>& tags, const std::string& creationDate) {
        // Start a transaction to ensure atomicity
        sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

        int noteID = id;

        if (noteID > -1) {
            // Update existing note if ID is valid (greater than -1)
            std::string sqlUpdate = "UPDATE Notes SET Title = ?, Content = ?, CreationDate = ? WHERE ID = ?;";
            sqlite3_stmt* stmtUpdate;

            int rc = sqlite3_prepare_v2(db, sqlUpdate.c_str(), -1, &stmtUpdate, nullptr);
            if (rc != SQLITE_OK) {
                std::cerr << "Failed to prepare note update statement: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
                return false;
            }

            sqlite3_bind_text(stmtUpdate, 1, title.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmtUpdate, 2, content.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmtUpdate, 3, creationDate.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmtUpdate, 4, noteID);  // Bind the note ID for the update

            rc = sqlite3_step(stmtUpdate);
            if (rc != SQLITE_DONE) {
                std::cerr << "Note update failed: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmtUpdate);
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
                return false;
            }

            sqlite3_finalize(stmtUpdate);
        }
        else {
            // Insert a new note if ID is -1 (no valid ID)
            std::string sqlInsert = "INSERT INTO Notes (Title, Content, CreationDate) VALUES (?, ?, ?);";
            sqlite3_stmt* stmtInsert;

            int rc = sqlite3_prepare_v2(db, sqlInsert.c_str(), -1, &stmtInsert, nullptr);
            if (rc != SQLITE_OK) {
                std::cerr << "Failed to prepare note insert statement: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
                return false;
            }

            sqlite3_bind_text(stmtInsert, 1, title.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmtInsert, 2, content.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmtInsert, 3, creationDate.c_str(), -1, SQLITE_STATIC);

            // Execute the insertion
            rc = sqlite3_step(stmtInsert);
            if (rc != SQLITE_DONE) {
                std::cerr << "Note insertion failed: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmtInsert);
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
                return false;
            }

            // Get the newly inserted note's ID
            noteID = sqlite3_last_insert_rowid(db);
            sqlite3_finalize(stmtInsert);
        }

        // Insert or update tags and NoteTags associations
        std::string sqlTagInsert = "INSERT OR IGNORE INTO Tags (Name) VALUES (?);";
        std::string sqlNoteTagInsert = "INSERT INTO NoteTags (NoteID, TagID) VALUES (?, ?);";
        sqlite3_stmt* stmtTagInsert;
        sqlite3_stmt* stmtNoteTagInsert;

        for (const std::string& tag : tags) {
            // Insert tag if it doesn't already exist
            int rc = sqlite3_prepare_v2(db, sqlTagInsert.c_str(), -1, &stmtTagInsert, nullptr);
            if (rc != SQLITE_OK) {
                std::cerr << "Failed to prepare tag insert statement: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
                return false;
            }

            sqlite3_bind_text(stmtTagInsert, 1, tag.c_str(), -1, SQLITE_STATIC);
            rc = sqlite3_step(stmtTagInsert);
            if (rc != SQLITE_DONE && rc != SQLITE_CONSTRAINT) {  // Ignore unique constraint errors
                std::cerr << "Tag insertion failed: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmtTagInsert);
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
                return false;
            }
            sqlite3_finalize(stmtTagInsert);

            // Get the tag ID
            std::string sqlGetTagID = "SELECT ID FROM Tags WHERE Name = ?;";
            sqlite3_prepare_v2(db, sqlGetTagID.c_str(), -1, &stmtTagInsert, nullptr);
            sqlite3_bind_text(stmtTagInsert, 1, tag.c_str(), -1, SQLITE_STATIC);
            rc = sqlite3_step(stmtTagInsert);
            int tagID = sqlite3_column_int(stmtTagInsert, 0);
            sqlite3_finalize(stmtTagInsert);

            // Insert into NoteTags
            rc = sqlite3_prepare_v2(db, sqlNoteTagInsert.c_str(), -1, &stmtNoteTagInsert, nullptr);
            if (rc != SQLITE_OK) {
                std::cerr << "Failed to prepare NoteTags insert statement: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
                return false;
            }

            sqlite3_bind_int(stmtNoteTagInsert, 1, noteID);
            sqlite3_bind_int(stmtNoteTagInsert, 2, tagID);

            // Execute the insertion
            rc = sqlite3_step(stmtNoteTagInsert);
            if (rc != SQLITE_DONE) {
                std::cerr << "NoteTags insertion failed: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmtNoteTagInsert);
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
                return false;
            }

            sqlite3_finalize(stmtNoteTagInsert);
        }

        // Commit the transaction
        sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
        std::cout << "Note and tags added/updated successfully!" << std::endl;

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
            return Note{id, noteTitle, content, {}, creationDate };
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
            return Note{id, title, content, {}, creationDate };
        }
        else {
            std::cerr << "No note found in the database." << std::endl;
        }

        sqlite3_finalize(stmt);
        return std::nullopt;
    }

    std::vector<Note> Database::getAllNotes() {
        std::vector<Core::Note> notes;

        // SQL to retrieve notes along with their associated tags
        std::string sql = "SELECT n.ID, n.Title, n.Content, n.CreationDate, t.Name "
            "FROM Notes n "
            "LEFT JOIN NoteTags nt ON n.ID = nt.NoteID "
            "LEFT JOIN Tags t ON nt.TagID = t.ID "
            "ORDER BY n.ID;";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to retrieve notes: " << sqlite3_errmsg(db) << std::endl;
            return notes;  // Return an empty vector on error
        }

        // Vector to hold the tags for the current note
        std::vector<std::string> tags;

        // Loop through the results
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string creationDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

            // Retrieve tag name (could be NULL if no tags)
            const char* tagName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

            // Check if we are at the start of a new note
            if (tags.empty() || notes.back().id != id) {
                // If it's a new note, create the tags vector
                tags.clear();  // Clear tags for the new note
                if (tagName) {
                    tags.push_back(tagName);  // Add the tag if it exists
                }
                // Create and add the new note
                Core::Note note(id, title, content, tags, creationDate);
                notes.push_back(note);
            }
            else {
                // If the note already exists, add the tag if it's not already in the list
                if (tagName && std::find(notes.back().tags.begin(), notes.back().tags.end(), tagName) == notes.back().tags.end()) {
                    notes.back().tags.push_back(tagName);  // Add the tag to the existing note
                }
            }
        }

        sqlite3_finalize(stmt);
        return notes;
    }

}
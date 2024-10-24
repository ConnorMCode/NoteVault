#pragma once
#include "Database.h"
#include "Note.h"
#include <string>
#include <optional>

namespace Core {

    class NoteManager {
    public:
        // Constructor to initialize the Database connection
        NoteManager(const std::string& dbName);

        // Add a note directly to the database
        void addNote(const Note& note);

        std::optional<Note> getNoteByTitle(const std::string& title);

        std::optional<Note> getRandomNote();

        std::vector<Note> getAllNotes();

    private:
        Database database; // Database object to interact with the SQLite database
    };

}
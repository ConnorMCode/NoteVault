#pragma once
#include "Database.h"
#include "Note.h"
#include <string>

namespace Core {

    class NoteManager {
    public:
        // Constructor to initialize the Database connection
        NoteManager(const std::string& dbName);

        // Add a note directly to the database
        void addNote(const Note& note);

    private:
        Database database; // Database object to interact with the SQLite database
    };

}
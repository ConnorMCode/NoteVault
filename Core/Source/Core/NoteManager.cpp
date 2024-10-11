#include "NoteManager.h"
#include <iostream>

namespace Core {

    // Constructor to initialize the Database connection
    NoteManager::NoteManager(const std::string& dbName)
        : database(dbName) {
        // Ensure the Notes table exists
        if (!database.createTables()) {
            std::cerr << "Error: Could not create table in the database.\n";
        }
        else {
            std::cout << "Notes table is ready." << std::endl;
        }
    }

    // Add a new note directly to the database
    void NoteManager::addNote(const Note& note) {
        if (!database.addNote(note.title, note.content, note.tags, note.creationDate)) {
            std::cerr << "Error: Could not add note to the database.\n";
        }
        else {
            std::cout << "Note added to the database successfully!" << std::endl;
        }
    }

}

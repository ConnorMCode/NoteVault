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
        std::cout << "Updating Note ID: " << note.id << ", Title: " << note.title << ", Content: " << note.content << std::endl;

        if (!database.addNote(note.id, note.title, note.content, note.tags, note.creationDate)) {
            std::cerr << "Error: Could not add note to the database.\n";
        }
        else {
            std::cout << "Note added to the database successfully!" << std::endl;
        }
    }

    std::optional<Note> NoteManager::getNoteByTitle(const std::string& title) {
        return database.getNoteByTitle(title);
    }

    std::optional<Note> NoteManager::getRandomNote() {
        return database.getRandomNote();
    }

    std::vector<Note> NoteManager::getAllNotes() {
        return database.getAllNotes();
    }

}

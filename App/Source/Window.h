#pragma once
#include <string>
#include <vector>
#include "Core/NoteManager.h" // Include the NoteManager for note-related actions

class Window {
public:
    Window(const std::string& dbName);
    void render(); // Function to render the GUI

private:
    Core::NoteManager noteManager; // NoteManager object to handle database operations
    std::string titleInput;
    std::string contentInput;
    std::string tagsInput;
    std::string creationDateInput;

    void addNote(); // Function to add the note to the database
};

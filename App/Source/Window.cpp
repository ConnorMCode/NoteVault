#include "Window.h"
#include "imgui.h"
#include <sstream> // To handle input conversion

// Constructor that initializes the NoteManager with the database
Window::Window(const std::string& dbName) : noteManager(dbName), titleInput(""), contentInput(""), tagsInput(""), creationDateInput("2024-10-09") {}

// The main function to render the GUI
void Window::render() {
    ImGui::Begin("Note Taking Application");

    // Input field for title
    ImGui::InputText("Title", &titleInput[0], 256);

    // Input field for content
    ImGui::InputTextMultiline("Content", &contentInput[0], 1024);

    // Input field for tags
    ImGui::InputText("Tags (comma-separated)", &tagsInput[0], 256);

    // Input field for creation date
    ImGui::InputText("Creation Date (YYYY-MM-DD)", &creationDateInput[0], 20);

    // Add note button
    if (ImGui::Button("Add Note")) {
        addNote();
    }

    ImGui::End();
}

// Function to add a note based on the input
void Window::addNote() {
    // Split the tags string into a vector
    std::vector<std::string> tags;
    std::stringstream ss(tagsInput);
    std::string tag;
    while (std::getline(ss, tag, ',')) {
        tags.push_back(tag);
    }

    // Create a new note
    Core::Note newNote(titleInput, contentInput, tags, creationDateInput);

    // Add the note to the database using the NoteManager
    noteManager.addNote(newNote);

    // Optionally clear the inputs after adding the note
    titleInput.clear();
    contentInput.clear();
    tagsInput.clear();
}

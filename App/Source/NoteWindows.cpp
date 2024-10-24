#include "NoteWindows.h"
#include "imgui.h"
#include "Core/Note.h"
#include <unordered_map>
#include <iostream>

void RenderNoteVaultWindow(Core::NoteManager& noteManager) {
    static bool firstFrameVault = true;
    if (firstFrameVault) {
        ImVec2 noteVaultPos = ImVec2(750, 200);  // Set starting position for NoteVault window
        ImVec2 noteVaultSize = ImVec2(500, 400);  // Set size for NoteVault window
        ImGui::SetNextWindowPos(noteVaultPos);
        ImGui::SetNextWindowSize(noteVaultSize);
        firstFrameVault = false;
    }

    ImGui::Begin("NoteVault");

    // If a note is selected (for editing)
    if (selectedNoteId != -1) {
        ImGui::Text("Editing Note ID: %d", selectedNoteId);
    }
    else {
        ImGui::Text("Creating a New Note");
    }

    // Input for the note content
    ImGui::InputTextMultiline("##editContent", input_text, IM_ARRAYSIZE(input_text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 6));

    // Submit button
    if (ImGui::Button("Submit")) {
        std::string creationDate = "2024-10-22";
        if (selectedNoteId != -1) {
            // Editing an existing note
            Core::Note editedNote(selectedNoteId, "Untitled", input_text, currentTags, creationDate);
            std::cout << "note: " << editedNote.id << " Now has content: " << editedNote.content << std::endl;
            noteManager.addNote(editedNote);    // Update the existing note
        }
        else {
            // Creating a new note
            Core::Note newNote("Untitled", input_text, currentTags, creationDate);
            noteManager.addNote(newNote);       // Add new note
        }

        // Clear after submission
        currentTags.clear();
        input_text[0] = '\0';            // Clear note input buffer
        submitted_text.clear();          // Clear submitted text
        selectedNoteId = -1;  // Reset to no selection
    }

    ImGui::End();
}

void RenderNoteExplorerWindow(Core::NoteManager& noteManager) {
    static bool firstFrameExplorer = true;
    if (firstFrameExplorer) {
        ImVec2 explorerPos = ImVec2(200, 200);  // Set starting position for Note Explorer window
        ImVec2 explorerSize = ImVec2(500, 400);  // Set size for Note Explorer window
        ImGui::SetNextWindowPos(explorerPos);
        ImGui::SetNextWindowSize(explorerSize);
        firstFrameExplorer = false;
    }

    std::vector<Core::Note> notes = noteManager.getAllNotes();
    ImGui::Begin("Note Explorer");

    // Button to add a new note
    if (ImGui::Button("Add Note")) {
        selectedNoteId = -1;  // No note selected for editing
        currentNoteContent = "";
        currentTags.clear();
        input_text[0] = '\0';            // Clear note input buffer
        submitted_text.clear();          // Clear submitted text
        selectedNoteId = -1;  // Reset to no selection
    }

    // Display existing notes
    for (auto& note : notes) {
        ImGui::PushID(note.id);

        // Create a selectable region for the note's content
        std::string noteDisplay = note.content;
        if (ImGui::Selectable(noteDisplay.c_str(), false, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, 30))) {
            // When clicked, load the note for editing in NoteVault
            selectedNoteId = note.id;
            currentNoteContent = note.content;
            currentTags = note.tags;
            std::strncpy(input_text, currentNoteContent.c_str(), sizeof(input_text));
        }

        ImGui::Separator();
        ImGui::PopID();
    }

    ImGui::End();
}

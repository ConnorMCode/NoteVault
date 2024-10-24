#pragma once
#include <string>
#include "Core/NoteManager.h"
#include "Core/Note.h"

// Declare functions for rendering the NoteVault and NoteExplorer windows
void RenderNoteVaultWindow(Core::NoteManager& noteManager);
void RenderNoteExplorerWindow(Core::NoteManager& noteManager);

static char             input_text[1024] = "";   // Input buffer for the text box
static std::string      submitted_text = "";    // Holds the submitted text
//static char             edit_input_text[1024] = "";  // Input buffer for the editing text box
//static std::string      edit_submitted_text = "";  // Holds the submitted edited text
static char tag_input[256] = "";           // Buffer for tag input

static int selectedNoteId = -1;  // No note selected by default
static std::string currentNoteContent;
static std::vector<std::string> currentTags;
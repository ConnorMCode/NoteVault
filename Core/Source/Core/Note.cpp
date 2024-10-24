#include "Note.h"
#include <iostream>

namespace Core {

    //Full Note constructor
    Note::Note(const std::string& title, const std::string& content, const std::vector<std::string>& tags, const std::string& creationDate)
        : id(-1), title(title), content(content), tags(tags), creationDate(creationDate) {}

    //Note constructor without title
    Note::Note(const std::string& content, const std::vector<std::string>& tags, const std::string& creationDate)
        : id(-1), content(content), tags(tags), creationDate(creationDate) {
        // Set the title as the first 30 characters of content
        if (content.length() <= 30) {
            title = content;
        }
        else {
            title = content.substr(0, 30); // Take first 30 characters of content
        }
    }

    Note::Note(int id, const std::string& title, const std::string& content, const std::vector<std::string>& tags, const std::string& creationDate)
        : id(id), title(title), content(content), tags(tags), creationDate(creationDate) {}

    void Note::editContent(const std::string& newContent) {
        content = newContent;
    }

    void Note::addTag(const std::string& tag) {
        tags.push_back(tag);
    }

}
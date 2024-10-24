#pragma once
#include <string>
#include <vector>

namespace Core {

	struct Note {
		std::string title;
		std::string content;
		std::vector<std::string> tags;
		std::string creationDate;
		int id;

		Note(const std::string& title, const std::string& content, const std::vector<std::string>& tags, const std::string& creationDate);

		Note(const std::string& content, const std::vector<std::string>& tags, const std::string& creationDate);

		Note(int id, const std::string& title, const std::string& content, const std::vector<std::string>& tags, const std::string& creationDate);

		void editContent(const std::string& newContent);
		void addTag(const std::string& tag);
	};

}
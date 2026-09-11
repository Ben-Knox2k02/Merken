#ifndef COURSE_H
#define COURSE_H

#include <string>

class Course {
	public:
		Course(std::string id, const std::string& name) : id(id), name(name) {}

		const std::string& GetId() const { return this->id; }
		const std::string& GetName() const { return this->name; }

		void UpdateName(const std::string& newName) {
			this->name = newName;
		}

	private:
		std::string id;
		std::string name;
};

#endif

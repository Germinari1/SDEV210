#ifndef Band_H
#define Band_H
#include <string>
#include <ostream>

/*
+ Band: The object representation of the bands.
*/
class Band {
private:
	int id;
	std::string name;
public:
	Band(int id, const std::string& name) : id(id), name(name) {}

	const int getID() {
		return id;
	}

	const std::string& getName() {
		return name;
	}

	// Overloading ostream so that we can get custom string printed out
	friend std::ostream& operator<<(std::ostream& os, const Band& band) {
		os << "<Band ID(" << band.id << "), name(" << band.name << ")/>";
		return os;
	}
};

#endif
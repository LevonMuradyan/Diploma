#ifndef PERSON_HPP
#define PERSON_HPP

#include <vector>
#include <string>
class Person {
private:
	std::string m_age;
	std::string m_gender;
public:
	Person();
	Person(std::string age, std::string gender);
	~Person();
	Person(const Person& person);
	std::string get_age() const ;
	std::string get_gender() const ;
	void set_age(const std::string& age);
	void set_gender(const std::string& gender);
	void print_person(const Person& person) const;
};

#endif PERSON_HPP
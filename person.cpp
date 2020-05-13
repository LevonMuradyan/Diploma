#include "person.h"
#include "functions.h"
#include <opencv2/opencv.hpp>

Person::Person() : m_gender("None"), m_age("0-0")
{

}

Person::Person(std::string age, std::string gender) :
	m_gender(gender), m_age(age)
{

}

Person::~Person()
{

}

Person::Person(const Person& person) :
	m_gender(person.m_gender), m_age(person.m_age)
{

}

std::string Person::get_age() const
{
    return m_age;
}

std::string Person::get_gender() const
{
    return m_gender;
}

void Person::set_age(const std::string& age)
{
    m_age = age;
}

void Person::set_gender(const std::string& gender)
{
    m_gender = gender;
}


void Person::print_person(const Person& person) const
{
    std::cout << "Age = " << person.m_age << std::endl;
    std::cout << "Gender = " << person.m_gender << std::endl;
}
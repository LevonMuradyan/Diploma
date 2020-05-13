#include <iostream>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <string>
#include "functions.hpp"
#include "person.hpp"
#define max_links 100

int main()
{
	std::string links_filename = "clothes/links.txt";
	std::vector<std::vector<std::string>> links (max_links, std::vector<std::string>(4));
	std::vector<Person> persons;
	read_text_from_file(links_filename, links);
	face_age_gender(persons);
	search(links, persons);
	cv::waitKey();
	return 0;
}
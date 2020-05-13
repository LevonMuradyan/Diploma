#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <tuple>
#include <opencv2/opencv.hpp>
#include "person.h"

std::tuple<cv::Mat, std::vector<std::vector<int>>> getFaceBox(cv::dnn::Net net,
	cv::Mat& frame, double conf_threshold);
void face_age_gender(std::vector<Person>& persons);
void search(const std::vector<std::vector<std::string>>& links,
	const std::vector<Person>& persons);
void read_text_from_file(const std::string& text_filename,
	std::vector<std::vector<std::string>>& text);
void print_vector(const std::vector<std::vector<std::string>>& links);
#endif // FUNCTIONS_HPP
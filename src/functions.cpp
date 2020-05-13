#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/core.hpp>
#include <tuple>
#include <iostream>
#include <iterator>
#include <iostream>
#include <chrono>
#include <random>
#include <set>
#include <cmath>
#include "functions.hpp"
#include <fstream>


std::tuple<cv::Mat, std::vector<std::vector<int>>> getFaceBox(cv::dnn::Net net, cv::Mat& frame, double conf_threshold)
{
    cv::Mat frameOpenCVDNN = frame.clone();
    int frameHeight = frameOpenCVDNN.rows;
    int frameWidth = frameOpenCVDNN.cols;
    double inScaleFactor = 1.0;
    cv::Size size = cv::Size(300, 300);
    // std::vector<int> meanVal = {104, 117, 123};
    cv::Scalar meanVal = cv::Scalar(104, 117, 123);

    cv::Mat inputBlob;
    inputBlob = cv::dnn::blobFromImage(frameOpenCVDNN, inScaleFactor, size, meanVal, true, false);

    net.setInput(inputBlob, "data");
    cv::Mat detection = net.forward("detection_out");

    cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

    std::vector<std::vector<int>> bboxes;

    for (int i = 0; i < detectionMat.rows; i++)
    {
        float confidence = detectionMat.at<float>(i, 2);

        if (confidence > conf_threshold)
        {
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frameWidth);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frameHeight);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frameWidth);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frameHeight);
            std::vector<int> box = { x1, y1, x2, y2 };
            bboxes.push_back(box);
            cv::rectangle(frameOpenCVDNN, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), 2, 4);
        }
    }

    return make_tuple(frameOpenCVDNN, bboxes);
}

void face_age_gender(std::vector<Person>& persons)
{
    std::string faceProto = "models/opencv_face_detector.pbtxt";
    std::string faceModel = "models/opencv_face_detector_uint8.pb";

    std::string ageProto = "models/age_deploy.prototxt";
    std::string ageModel = "models/age_net.caffemodel";

    std::string genderProto = "models/gender_deploy.prototxt";
    std::string genderModel = "models/gender_net.caffemodel";

    cv::Scalar MODEL_MEAN_VALUES = cv::Scalar(78.4263377603, 87.7689143744, 114.895847746); // ?

    std::vector<std::string> ageList = { "(0-2)", "(4-6)", "(8-12)", "(15-20)", "(25-32)",
      "(38-43)", "(48-53)", "(60-100)" };

    std::vector<std::string> genderList = { "Male", "Female" };

    // Load Network
    cv::dnn::Net ageNet = cv::dnn::readNet(ageModel, ageProto); // ?
    cv::dnn::Net genderNet = cv::dnn::readNet(genderModel, genderProto); // ?
    cv::dnn::Net faceNet = cv::dnn::readNet(faceModel, faceProto); // ?

    cv::VideoCapture cap; // ?
    cap.open("images/couple1.jpg");
    int padding = 20;
    while (cv::waitKey(1) < 0) {
        // read frame
        cv::Mat frame;
        cap.read(frame);
        if (frame.empty())
        {
            cv::waitKey(); // ?
            break;
        }

        std::vector<std::vector<int>> bboxes;
        cv::Mat frameFace;
        std::tie(frameFace, bboxes) = getFaceBox(faceNet, frame, 0.7); // ?

        if (bboxes.size() == 0) {
            std::cout << "No face detected, checking next frame." << std::endl;
            continue;
        }
        for (auto it = begin(bboxes); it != end(bboxes); ++it) {
            cv::Rect rec(it->at(0) - padding, it->at(1) - padding, it->at(2) - it->at(0) + 2 * padding, it->at(3) - it->at(1) + 2 * padding);
            cv::Mat face = frame(rec); // take the ROI of box on the frame

            cv::Mat blob;
            blob = cv::dnn::blobFromImage(face, 1, cv::Size(227, 227), MODEL_MEAN_VALUES, false);
            genderNet.setInput(blob);
            // string gender_preds;
            std::vector<float> genderPreds = genderNet.forward();
            // printing gender here
            // find max element index
            // distance function does the argmax() work in C++
            int max_index_gender = std::distance(genderPreds.begin(), max_element(genderPreds.begin(), genderPreds.end()));
            std::string gender = genderList[max_index_gender];
            std::cout << "Gender: " << gender << std::endl;

            /* // Uncomment if you want to iterate through the gender_preds vector
            for(auto it=begin(gender_preds); it != end(gender_preds); ++it) {
              cout << *it << endl;
            }
            */

            ageNet.setInput(blob);
            std::vector<float> agePreds = ageNet.forward();
            /* // uncomment below code if you want to iterate through the age_preds
             * vector
            cout << "PRINTING AGE_PREDS" << endl;
            for(auto it = age_preds.begin(); it != age_preds.end(); ++it) {
              cout << *it << endl;
            }*/


            // finding maximum indicd in the age_preds vector
            int max_indice_age = std::distance(agePreds.begin(), max_element(agePreds.begin(), agePreds.end()));
            std::string age = ageList[max_indice_age];
            std::cout << "Age: " << age << std::endl;
            persons.push_back(Person(age, gender));
            std::string label = gender + ", " + age; // label
            cv::putText(frameFace, label, cv::Point(it->at(0), it->at(1) - 15), cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(0, 255, 255), 2, cv::LINE_AA);
            //imshow("Frame", frameFace);
            //imwrite("output/out.jpg", frameFace);
        }
    }
}

void search(const std::vector<std::vector<std::string>>& links, const std::vector<Person>& persons)
{
    for (size_t i = 0; i < links.size(); ++i)
    {
        for (size_t j = 0; j < persons.size(); ++j)
        {
            if (persons[j].get_gender() == links[i][0])
            {
                if (persons[j].get_age() == "(" + links[i][1] + ")")
                {
                    system(("start " + links[i][3]).c_str());
                }
            }
        }
    }
}

void read_text_from_file(const std::string& text_filename, std::vector<std::vector<std::string>>& text)
{
    auto start = std::chrono::steady_clock::now();
    std::ifstream file(text_filename);
    size_t counter = 0;
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            //std::cout << line << " " << line.size() << std::endl;
            size_t size = line.size();
            char* str = &line[0];
            char* pch;
            pch = strtok(str, " ");

            //std::cout << line << " " << line.size() << std::endl;

            for(size_t i = 0; i < 4; ++i)
            {
                //std::cout << pch << " " << strlen(pch) << std::endl;
                text[counter][i] = pch;
                pch = strtok(NULL, " ");
            }

            counter++;
        }
        text.resize(counter);
        file.close();
    }
    else
    {
        std::cerr << "Couldn't open " << text_filename << " for reading\n";
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
}

void print_vector(const std::vector<std::vector<std::string>>& links)
{
    for (size_t i = 0; i < links.size(); ++i)
    {
        system(("start " + links[i][3]).c_str());
        for (size_t j = 0; j < links[i].size(); ++j)
        {
            std::cout << links[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
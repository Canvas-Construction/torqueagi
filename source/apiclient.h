#pragma once
#include <filesystem>
#include <string>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

// API Client for TorqueAGI
class APIClient {
public:
    APIClient(const std::string& url, const std::string& key) : api_url_(url), api_key_(key) {}

    // Request from an image file
    json DetectSeams(const fs::path& image_path);
    // Request from an image buffer
    json DetectSeams(const cv::Mat& image, const std::string& id_dummy="test");

private:
    std::string api_url_;
    std::string api_key_;
};

#include "apiclient.h"
#include <iostream>
#include <fstream>

json APIClient::SendRequest(std::vector<httplib::MultipartFormData>& items) {
    // Create client
    httplib::Client cli(api_url_);

    // Header
    httplib::Headers headers = {{"X-API-Key", api_key_}};

    auto res = cli.Post(api_endpoint_, headers, items);

    if (!res) {
        std::cerr << "API request failed: " << res.error() << std::endl;
        return {{"error", "API request failed"}};
    }
    if (res->status != 200) {
        std::cerr << "API error: " << res->status << " " << res->body << std::endl;
        return {{"error", "Bad status"}};
    }

    std::cout << "Raw JSON response:" << std::endl;
    std::cout << res->body << std::endl;
    return json::parse(res->body);
}

json APIClient::DetectSeams(const fs::path& image_path) {
    // Load file
    std::ifstream ifs(image_path, std::ios::binary);
    if (!ifs) {
        std::cerr << "Failed to open image: " << image_path << std::endl;
        return {{"error", "Failed to open image"}};
    }
    std::string image_data((std::istreambuf_iterator<char>(ifs)),
                           std::istreambuf_iterator<char>());

    // Prepare multipart items
    httplib::MultipartFormData items = {
        {"file", image_data, image_path.filename().string(), "application/octet-stream"},
        {"metadata", "{}", "", "application/json"}};

    return SendRequest(items);
}

json APIClient::DetectSeams(const cv::Mat& image, const std::string& id_dummy) {
    // OpenCV → PNG
    std::vector<uchar> buf;
    cv::imencode(".png", image, buf);
    std::string image_data(reinterpret_cast<char*>(buf.data()), buf.size());

    // multipart/form-data
    httplib::MultipartFormData items = {
        { "file", image_data, id_dummy + ".png", "image/png" },
        { "metadata", "{}", "", "application/json" }};

    return SendRequest(items);
}
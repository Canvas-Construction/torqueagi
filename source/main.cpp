#include <iostream>
#include <filesystem>
#include "apiclient.h"
#include "visualizer.h"

namespace fs = std::filesystem;

// Configuration
const std::string API_URL       = "http://192.168.2.1:9000";
const std::string API_ENDPOINT  = "/canvas";

int main(int argc, char* argv[]) {
    if(argc < 3) {
        std::cout << "Usage: " << argv[0] << " <image_file> <api_key>" << std::endl;
        return 1;
    }

    fs::path image_path = argv[1];
    std::string api_key = argv[2];

    if(!fs::exists(image_path) || !fs::is_regular_file(image_path)) {
        std::cerr << "Invalid file: " << image_path << std::endl;
        return 1;
    }

    APIClient client(API_URL, API_ENDPOINT, api_key);
    Visualizer visualizer;

    std::cout << "Processing image: " << image_path.filename().string() << std::endl;

    //json result = client.DetectSeams(image_path);
        
    cv::Mat img_buf = cv::imread(image_path.string());
    json result = client.DetectSeams(img_buf);

    if(result.contains("error")) {
        std::cerr << "API error: " << result["error"] << std::endl;
        return 1;
    }

    fs::path output_path = image_path.parent_path() / ("output_" + image_path.filename().string());

    visualizer.DrawSeamDetections(image_path, result, output_path);


    return 0;
}
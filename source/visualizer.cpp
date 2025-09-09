#include "visualizer.h"
#include <iostream>
#include <opencv2/opencv.hpp>

void Visualizer::DrawSeamDetections(const fs::path& image_path, const json& result, const fs::path& output_path) {
    cv::Mat img = cv::imread(image_path.string());
    if(img.empty()) {
        std::cerr << "Failed to load image: " << image_path << std::endl;
        return;
    }

    auto content = result["result"][0];

    std::cout << "Total detections: " << content["total_detections"].get<int>() << std::endl;

    if(content.contains("detections")) {
        auto detections = content["detections"];
        auto bboxes = detections["bboxes"];
        auto labels = detections["labels"];

        for(size_t i = 0; i < bboxes.size(); i++) {
            auto bbox = bboxes[i];
            std::string label = labels[i];

            int x1 = bbox[0].get<float>() * img.cols;
            int y1 = bbox[1].get<float>() * img.rows;
            int x2 = bbox[2].get<float>() * img.cols;
            int y2 = bbox[3].get<float>() * img.rows;

            std::cout << x1 << " " << y1 << " " << x2 << " " << y2 << " " << label << std::endl;

            cv::Scalar color;
            if(label == "H_SEAM") {
                color = cv::Scalar(0, 170, 0); // Green for horizontal seams (high visibility)
            } else if(label == "V_SEAM") {
                color = cv::Scalar(255, 0, 0); // Blue for vertical seams (distinct from horizontal)
            } else if(label == "background") {
                color = cv::Scalar(0, 128, 255); // Orange for background elements (neutral contrast)
            } else {
                color = cv::Scalar(255,255,255); // White for unknown
            }

            cv::rectangle(img, cv::Point(x1,y1), cv::Point(x2,y2), color, 2);
        }
    }
    cv::imwrite(output_path.string(), img);
    std::cout << "Saved result: " << output_path << "\n";
}

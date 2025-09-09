#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

// Visualizer class for drawing detections
class Visualizer {
public:
    void DrawSeamDetections(const fs::path& image_path, const json& result, const fs::path& output_path);
};

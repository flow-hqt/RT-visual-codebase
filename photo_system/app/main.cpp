#include "rt_vision/image_system.h"
#include "rt_vision/gui_manager.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

struct UserConfig {
    bool doCrop = false;
    int cropX, cropY, cropW, cropH;

    bool doRotate = false;
    int rotateAngle = 0;

    bool convertFormat = false;
    std::string targetExt;
};

UserConfig getUserConfiguration() {
    UserConfig config;
    char choice;
    std::cout << "\n=== IMAGE PROCESSOR ===\n";

    // 1. 裁剪
    std::cout << "[1] Crop? (y/n): "; std::cin >> choice;
    if (choice == 'y') {
        config.doCrop = true;
        std::cout << "   X Y W H (space separated): ";
        std::cin >> config.cropX >> config.cropY >> config.cropW >> config.cropH;
    }

    // 2. 旋转
    std::cout << "[2] Rotate 90 deg? (y/n): "; std::cin >> choice;
    if (choice == 'y') {
        config.doRotate = true;
        config.rotateAngle = 90; // 目前算法只写了90度
    }

    // 3. 格式转换
    std::cout << "[3] Convert Format? (y/n): "; std::cin >> choice;
    if (choice == 'y') {
        config.convertFormat = true;
        std::cout << "   Target (png/jpg/bmp): ";
        std::cin >> config.targetExt;
        if (config.targetExt[0] != '.') config.targetExt = "." + config.targetExt;
    }
    return config;
}

int main() {
    using namespace RtVision;

    // 1. 选择文件 (支持多格式)
    std::string trainDir = fs::current_path().string() + "/train1";
    if (!fs::exists(trainDir)) fs::create_directories(trainDir);
    
    // 你需要往 train1 里放 jpg, png, bmp, webp 各一张来测试
    std::cout << "Select images (supports jpg, png, bmp, webp)...\n";
    auto selectedFiles = GuiManager::selectImages(trainDir);

    if (selectedFiles.empty()) return 0;

    // 2. 获取操作配置
    UserConfig config = getUserConfiguration();

    // 3. 建立管线
    std::vector<BatchManager::Task> pipeline;

    if (config.doCrop) {
        pipeline.push_back([config](std::shared_ptr<Image> img) {
            ImageProcessor::crop(img, config.cropX, config.cropY, config.cropW, config.cropH);
        });
    }

    if (config.doRotate) {
        pipeline.push_back([config](std::shared_ptr<Image> img) {
            ImageProcessor::rotate(img, config.rotateAngle);
        });
    }

    // 4. 执行
    BatchManager manager;
    manager.processBatch(selectedFiles, "./output_v2", pipeline, config.targetExt);

    GuiManager::showMessage("Success", "Processed images saved to output_v2");
    return 0;
}
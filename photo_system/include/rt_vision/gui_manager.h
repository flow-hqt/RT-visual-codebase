#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>

// 引入我们刚下载的库
#include "portable-file-dialogs.h"

namespace RtVision {

    class GuiManager {
    public:
        // 核心功能：弹出GUI让用户选择文件
        static std::vector<std::string> selectImages(const std::string& startDir) {
            // 检查目录是否存在
            if (!std::filesystem::exists(startDir)) {
                pfd::message("Error", "Directory not found: " + startDir, pfd::choice::ok, pfd::icon::error).result();
                return {};
            }

            // 1. 检查是否可用（初始化）
            if (!pfd::settings::available()) {
                std::cerr << "Portable File Dialogs not available on this platform." << std::endl;
                return {};
            }

            // 2. 弹出原生文件选择窗口
            // 参数：标题，默认路径，过滤器，模式(多选)
            auto selections = pfd::open_file(
                "Select Images to Process (Ctrl+A to Select All)", 
                startDir,
                { "Image Files", "*.jpg *.png *.jpeg *.bmp*.webp", "All Files", "*" },
                pfd::opt::multiselect
            ).result();

            return selections;
        }

        // 简单的进度反馈 GUI (可选)
        static void showMessage(const std::string& title, const std::string& msg) {
            pfd::message(title, msg, pfd::choice::ok, pfd::icon::info).result();
        }
    };
}
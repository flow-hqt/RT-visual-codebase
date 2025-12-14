#include "rt_vision/image_system.h"
#include <filesystem>
#include <thread>
#include <mutex>
#include <cmath>
#include <cstring> 
#include <future>  // <--- 修正1: 必须显式包含这个库

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace fs = std::filesystem;

namespace RtVision {

    // --- Image Base ---
    Image::~Image() {
        if (data) stbi_image_free(data);
    }

    void Image::updateData(unsigned char* newData, int newW, int newH, int newCh) {
        if (data) stbi_image_free(data); 
        data = newData;                 
        width = newW;
        height = newH;
        channels = newCh;
    }

    bool Image::save(const std::string& filepath) {
        std::string ext = fs::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        int stride = width * channels;
        int res = 0;

        if (ext == ".png") {
            res = stbi_write_png(filepath.c_str(), width, height, channels, data, stride);
        } else if (ext == ".jpg" || ext == ".jpeg") {
            res = stbi_write_jpg(filepath.c_str(), width, height, channels, data, 90);
        } else if (ext == ".bmp") {
            res = stbi_write_bmp(filepath.c_str(), width, height, channels, data);
        } else {
            std::cerr << "Warning: Saving " << ext << " is not supported. Falling back to PNG." << std::endl;
            std::string fixPath = filepath + ".png";
            res = stbi_write_png(fixPath.c_str(), width, height, channels, data, stride);
        }
        return res != 0;
    }

    // --- 修正2: 各格式 Load 实现 (独立实现，不再互相调用) ---
    
    // 提取一个通用的内部辅助函数，避免代码重复
    // 注意：这不是类成员，只是文件内部的静态函数
    static unsigned char* common_load(const std::string& path, int* w, int* h, int* ch) {
        unsigned char* d = stbi_load(path.c_str(), w, h, ch, 0);
        if (d) {
            std::cout << "[LOAD] Loaded: " << fs::path(path).filename().string() 
                      << " (" << *w << "x" << *h << ")" << std::endl;
        } else {
            std::cerr << "[LOAD] Failed: " << path << std::endl;
        }
        return d;
    }

    bool JpgImage::load(const std::string& path) {
        filename = fs::path(path).filename().string();
        data = common_load(path, &width, &height, &channels);
        return data != nullptr;
    }

    bool PngImage::load(const std::string& path) {
        filename = fs::path(path).filename().string();
        data = common_load(path, &width, &height, &channels);
        return data != nullptr;
    }

    bool BmpImage::load(const std::string& path) {
        filename = fs::path(path).filename().string();
        data = common_load(path, &width, &height, &channels);
        return data != nullptr;
    }

    bool WebpImage::load(const std::string& path) {
        filename = fs::path(path).filename().string();
        data = common_load(path, &width, &height, &channels);
        return data != nullptr;
    }

    // --- Factory ---
    std::shared_ptr<Image> ImageFactory::create(const std::string& filepath) {
        std::string ext = fs::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".jpg" || ext == ".jpeg") return std::make_shared<JpgImage>();
        if (ext == ".png") return std::make_shared<PngImage>();
        if (ext == ".bmp") return std::make_shared<BmpImage>();
        if (ext == ".webp") return std::make_shared<WebpImage>();
        
        return nullptr;
    }

    // --- Processors ---
    
    void ImageProcessor::crop(std::shared_ptr<Image> img, int x, int y, int w, int h) {
        if (!img->getData()) return;
        
        int oldW = img->getWidth();
        int oldH = img->getHeight();
        if (x < 0 || y < 0 || x + w > oldW || y + h > oldH) {
            std::cerr << "Crop region out of bounds!" << std::endl;
            return;
        }

        std::cout << "-> Cropping " << img->getName() << " to " << w << "x" << h << std::endl;

        int ch = img->getChannels();
        unsigned char* newData = (unsigned char*)malloc(w * h * ch);

        for (int i = 0; i < h; ++i) {
            int srcIdx = ((y + i) * oldW + x) * ch;
            int dstIdx = (i * w) * ch;
            memcpy(newData + dstIdx, img->getData() + srcIdx, w * ch);
        }

        img->updateData(newData, w, h, ch);
    }

    void ImageProcessor::rotate(std::shared_ptr<Image> img, int angle) {
        if (!img->getData()) return;
        
        if (angle != 90) {
            std::cout << "Simple rotation only supports 90 degrees for now." << std::endl;
            return;
        }

        std::cout << "-> Rotating " << img->getName() << " 90 degrees" << std::endl;

        int w = img->getWidth();
        int h = img->getHeight();
        int ch = img->getChannels();

        unsigned char* newData = (unsigned char*)malloc(w * h * ch);
        
        for (int y = 0; y < w; ++y) {       
            for (int x = 0; x < h; ++x) {   
                int dstIdx = (y * h + x) * ch;
                int srcX = y;
                int srcY = h - 1 - x;
                int srcIdx = (srcY * w + srcX) * ch;
                memcpy(newData + dstIdx, img->getData() + srcIdx, ch);
            }
        }

        img->updateData(newData, h, w, ch); 
    }

    void ImageProcessor::toGrayscale(std::shared_ptr<Image> img) { 
        // 简单实现占位
        if(img->getData()) std::cout << "-> ToGrayscale (Mock)" << std::endl;
    }
    void ImageProcessor::resize(std::shared_ptr<Image> img, int w, int h) { 
        // 简单实现占位
        if(img->getData()) std::cout << "-> Resize (Mock)" << std::endl;
    }

    // --- BatchManager ---
    std::mutex print_mutex;
    void BatchManager::processBatch(const std::vector<std::string>& filepaths, 
                                    const std::string& outputDir,
                                    std::vector<Task> operations,
                                    std::string overrideExtension) {
        if (!fs::exists(outputDir)) fs::create_directories(outputDir);
        
        // 这里的 std::future 需要 <future> 头文件
        std::vector<std::future<void>> futures;

        for (const auto& path : filepaths) {
            // std::async, std::launch 需要 <future> 头文件
            futures.push_back(std::async(std::launch::async, [=]() {
                auto img = ImageFactory::create(path);
                if (!img) return; 
                
                {
                    std::lock_guard<std::mutex> lock(print_mutex);
                    img->load(path);
                }

                for (auto& op : operations) op(img);

                std::string filename = img->getName();
                if (!overrideExtension.empty()) {
                    filename = fs::path(filename).replace_extension(overrideExtension).string();
                }
                
                std::string outPath = outputDir + "/processed_" + filename;
                img->save(outPath); 
                
                {
                    std::lock_guard<std::mutex> lock(print_mutex);
                    std::cout << "Saved: " << outPath << std::endl;
                }
            }));
        }
        for (auto& f : futures) f.get();
    }
}
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <iostream>

namespace RtVision {

    // ==========================================
    // 1. 图片基类 (增强版)
    // ==========================================
    class Image {
    public:
        virtual ~Image();
        
        // 核心接口
        virtual bool load(const std::string& filepath) = 0;
        
        // 通用保存接口 (支持 jpg, png, bmp, tga)
        bool save(const std::string& filepath);
        
        // Getters
        int getWidth() const { return width; }
        int getHeight() const { return height; }
        int getChannels() const { return channels; } // 新增：获取通道数
        std::string getName() const { return filename; }
        unsigned char* getData() { return data; }

        // 核心：更新缓冲区 (用于裁剪/旋转后替换旧数据)
        void updateData(unsigned char* newData, int newW, int newH, int newCh);

    protected:
        std::string filename;
        int width = 0, height = 0, channels = 0;
        unsigned char* data = nullptr;
    };

    // ==========================================
    // 2. 格式派生类 (新增 BMP 和 WEBP)
    // ==========================================
    class JpgImage : public Image { public: bool load(const std::string& path) override; };
    class PngImage : public Image { public: bool load(const std::string& path) override; };
    
    // 新增支持的格式
    class BmpImage : public Image { public: bool load(const std::string& path) override; };
    class WebpImage : public Image { public: bool load(const std::string& path) override; };

    // 工厂模式 (升级版)
    class ImageFactory {
    public:
        static std::shared_ptr<Image> create(const std::string& filepath);
    };

    // ==========================================
    // 3. 处理操作模板 (新增 Crop 和 Rotate 算法)
    // ==========================================
    class ImageProcessor {
    public:
        static void toGrayscale(std::shared_ptr<Image> img);
        static void resize(std::shared_ptr<Image> img, int w, int h);
        
        // 旋转 (简单实现：90度倍数，或最近邻插值)
        static void rotate(std::shared_ptr<Image> img, int angle);

        // 裁剪 (Crop)
        static void crop(std::shared_ptr<Image> img, int x, int y, int w, int h);
    };

    // ==========================================
    // 4. 并发管理器 (保持不变，复用逻辑)
    // ==========================================
    class BatchManager {
    public:
        using Task = std::function<void(std::shared_ptr<Image>)>;
        void processBatch(const std::vector<std::string>& filepaths, 
                          const std::string& outputDir,
                          std::vector<Task> operations,
                          std::string overrideExtension = "");
    };
}
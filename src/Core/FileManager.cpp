#include "../../thirdparty/stb_image.h"
#include "../../thirdparty/stb_image_write.h"
#include "../../include/Core/FileManager.hpp"
#include <iostream>
#include <giomm/file.h>


namespace Editor
{
    std::unique_ptr<Document> FileManager::Load(const std::filesystem::path& path)
    {
        int width, height, channels;

        unsigned char* data = stbi_load(
            path.string().c_str(),
            &width,
            &height,
            &channels,
            4
        );

        if (!data)
            throw std::runtime_error("Failed to load image");

        const size_t pixelCount = width * height;

        std::vector<Pixel> pixels(pixelCount);
        std::memcpy(pixels.data(), data, pixelCount * sizeof(Pixel));
        stbi_image_free(data);

        Image image(width, height, channels, std::move(pixels));

        auto doc = std::make_unique<Document>(width, height, std::move(image));
        doc->SetFilePath(path);
        return doc;
    }

    const std::vector<std::filesystem::path>& FileManager::GetRecentFiles() { return m_recentFiles; }

    bool FileManager::Save(const Document& doc)
    {
        return SaveAs(doc, doc.GetFilePath());
    }

    bool FileManager::SaveAs(const Document& doc, const std::filesystem::path& path)
    {
        try {
            const Image& img = doc.GetImage();
            const auto& pixels = img.GetPixelData();

            if (pixels.empty()) return false;

            std::filesystem::path finalPath = path;
            if (!finalPath.has_extension()) {
                finalPath.replace_extension(".png");
            }

            std::string ext = finalPath.extension().string();
            std::ranges::transform(ext, ext.begin(), ::tolower);

            const uint8_t* raw_data = reinterpret_cast<const uint8_t*>(pixels.data());

            int width = img.GetWidth();
            int height = img.GetHeight();
            int channels = 4;
            int stride = width * channels;

            int result = 0;
            if (ext == ".png") {
                result = stbi_write_png(finalPath.string().c_str(), width, height, channels, raw_data, stride);
            }
            else if (ext == ".jpg" || ext == ".jpeg") {
                result = stbi_write_jpg(finalPath.string().c_str(), width, height, channels, raw_data, 90);
            }

            return result != 0;
        }
        catch (const std::exception& e) {
            std::cerr << "Save error: " << e.what() << std::endl;
            return false;
        }
    }
}
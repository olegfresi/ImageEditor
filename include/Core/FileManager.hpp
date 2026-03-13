/*
 * Project: ImageEditor
 * File: File.hpp
 * Author: olegfresi
 * Created: 03/03/26 11:44
 * 
 * Copyright © 2026 olegfresi
 * 
 * Licensed under the MIT License. You may obtain a copy of the License at:
 * 
 *     https://opensource.org/licenses/MIT
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once
#include <filesystem>
#include <memory>
#include "Document.hpp"

namespace Editor
{
    class FileManager
    {
    public:
        /**
        * Default constructor.
        *
        * Initializes the FileManager with default settings.
        */
        FileManager() = default;

        /**
        * Copy constructor deleted.
        *
        * FileManager cannot be copied to prevent resource conflicts.
        */
        FileManager(const FileManager&) = delete;

        /**
        * Copy assignment operator deleted.
        *
        * FileManager cannot be copy-assigned to prevent resource conflicts.
        */
        FileManager& operator=(const FileManager&) = delete;

        /**
        * Load image from file.
        *
        * Reads image data from disk using the stb_image library.
        * Supported formats: PNG, JPG, BMP, TGA, and others.
        * Converts loaded image to RGBA format internally.
        *
        * @param path Path to the image file
        * @return A unique_ptr to the document containing the loaded image
        */
        static std::unique_ptr<Document> Load(const std::filesystem::path& path);

        /**
        * Get the list of recently opened files.
        *
        * Returns a list of file paths that have been recently loaded or saved.
        *
        * @return A constant reference to the vector of recent file paths.
        */
        const std::vector<std::filesystem::path>& GetRecentFiles();

        /**
        * Save the document to its current file path.
        *
        * Saves the document to the path it was loaded from or last saved to.
        * If no path is set, the operation fails.
        *
        * @param doc The document to save.
        * @return True if the save was successful, false otherwise.
        */
        static bool Save(const Document& doc);

        /**
        * Save the document to a specified file path.
        *
        * Saves the document to the given path, updating the document's file path.
        *
        * @param doc The document to save.
        * @param path The file path to save to.
        * @return True if the save was successful, false otherwise.
        */
        static bool SaveAs(const Document& doc, const std::filesystem::path& path);

    private:
        std::vector<std::filesystem::path> m_recentFiles;
    };
}
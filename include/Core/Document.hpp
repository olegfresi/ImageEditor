/*
 * Project: ImageEditor
 * File: Document.hpp
 * Author: olegfresi
 * Created: 03/03/26 11:41
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
#include <stack>
#include <vector>
#include "Layer.hpp"
#include "../../include/Command/Command.hpp"
#include "../Tools/Brush.hpp"


/**
* Color space format for document representation.
*
* Defines the color model used for image processing and display.
*/
enum class ColorSpace
{
    RGB,
    CMYK,
    Grayscale
};


namespace Editor
{
    /**
    * Main document class for managing layers, editing state, and file I/O.
    *
    * Represents a complete image editing document with support for multiple layers,
    * undo/redo functionality, and color space management.
    * Tracks whether the document has been modified (dirty flag).
    */
    class Document
    {
    public:

        /**
        * Constructor with specified dimensions and color space.
        *
        * Creates a new document with the given width, height, and color space.
        * Initializes the document with default layers and settings.
        *
        * @param width The width of the document in pixels.
        * @param height The height of the document in pixels.
        * @param image The image to use
        * @param color The color space for the document, defaults to RGB.
        */
        Document(int width, int height, Image image, ColorSpace color = ColorSpace::RGB);

        /**
        * Save the document to its current file path.
        *
        * Saves the document using the FileManager. If no path is set, the operation may fail.
        */
        void Save();

        /**
        * Save the document to a specified file path.
        *
        * Saves the document to the given path using the FileManager and updates the document's file path.
        *
        * @param path The file path to save to.
        */
        void SaveAs(const std::filesystem::path& path);

        /**
        * Execute a command on the document.
        *
        * Applies the given command to the document, updating its state and adding it to the undo stack.
        *
        * @param command The command to execute.
        */
        void ExecuteCommand(std::unique_ptr<Command::ICommand> command);

        /**
        * Undo the last executed command.
        *
        * Reverts the last executed command, restoring the previous state of the document.
        */
        void Undo();

        /**
        * Redo the last undone command.
        *
        * Re-applies the last undone command, advancing the document state forward.
        */
        void Redo();

        /**
        * Set the file path of the document.
        *
        * Used when a document is created from an existing file on disk.
        *
        * @param path The file path to associate with this document.
        */
        void SetFilePath(const std::filesystem::path& path) { m_filePath = path; }

        void SetOnImageChangedCallback(std::function<void()> callback) { m_onImageChanged = std::move(callback); }

        void SetOnCommandStackChangedCallback(std::function<void()> cb) { m_onCommandStackChanged = std::move(cb); }

        /**
        * Check if the document has unsaved changes.
        *
        * @return True if the document has been modified since last save, false otherwise.
        */
        [[nodiscard]] bool IsDirty() const noexcept;

        [[nodiscard]] bool CanUndo() const noexcept { return !m_undoStack.empty(); }

        [[nodiscard]] bool CanRedo() const noexcept { return !m_redoStack.empty(); }

        /**
        * Get the image associated with the document.
        *
        * Provides read-only access to the document's image data.
        *
        * @return A constant reference to the document's image.
        */
        [[nodiscard]] const Image& GetImage() const noexcept { return m_image; }

        /**
        * Get the file path of the document.
        *
        * @return The file path associated with the document.
        */
        [[nodiscard]] std::filesystem::path GetFilePath() const noexcept { return m_filePath; }

        /**
        * Get the image associated with the document (non-const).
        *
        * Provides read-write access to the document's image data for modification.
        *
        * @return A reference to the document's image.
        */
        [[nodiscard]] Image& GetImage() noexcept { return m_image; }

        void InvalidatePreview()  noexcept { m_previewImage = Image{}; }

    private:
        int m_width{};
        int m_height{};
        int m_revision{};

        bool m_isDirty{false};
        bool m_isPreviewActive{false};

        size_t m_saveIndex = 0;

        std::filesystem::path m_filePath{};

        ColorSpace m_colorSpace = ColorSpace::RGB;

        Image m_image;
        Image m_previewImage;

        std::function<void()> m_onImageChanged;
        std::function<void()> m_onCommandStackChanged;

        std::vector<Pixel> m_previewBase;
        std::vector<Layer> m_layers{};
        std::vector<std::unique_ptr<Command::ICommand>> m_undoStack{};
        std::vector<std::unique_ptr<Command::ICommand>> m_redoStack{};

    private:
        void NotifyImageChanged() const;
        void UpdateDirtyFlag();
    };
}
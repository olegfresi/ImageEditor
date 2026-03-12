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
#include <stack>
#include <vector>
#include "Layer.hpp"
#include "Brush.hpp"
#include "File.hpp"

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
        * Default constructor.
        *
        * Creates a new empty document with default settings.
        * No file path is associated with the document.
        */
        Document();

        /**
        * Constructor from File object.
        *
        * Creates a document and loads image data from a File object.
        * Initializes layers and document metadata from the file.
        *
        * @param file The File object containing image data to load
        */
        Document(const File& file);

        /**
        * Constructor from file path.
        *
        * Creates a document and loads image data from the specified file path.
        * Sets the document's file path for future save operations.
        *
        * @param filePath Path to the image file to load
        */
        explicit Document(std::filesystem::path filePath);

        /**
        * Save document to its current file path.
        *
        * Writes all layers and document data to the file at m_filePath.
        * If no file path is set, the save operation may fail.
        * Clears the dirty flag on successful save.
        */
        void Save();

        /**
        * Save document to a new file path.
        *
        * Writes all layers and document data to the specified file path.
        * Updates the document's m_filePath to the new location.
        * Clears the dirty flag on successful save.
        *
        * @param path The new file path to save to
        */
        void SaveAs(const std::filesystem::path& path);

        /**
        * Add a new layer to the document.
        *
        * Creates and appends a new empty layer to the layer stack.
        * The new layer becomes the active layer for editing.
        * Sets the dirty flag.
        */
        void AddLayer();

        /**
        * Remove a layer from the document.
        *
        * Removes the layer at the specified index from the layer stack.
        * Cannot remove all layers (at least one must remain).
        * Sets the dirty flag.
        *
        * @param index Index of the layer to remove (0-based)
        */
        void RemoveLayer(int index);

        /**
        * Apply brush stroke to the active layer.
        *
        * Applies the specified brush at the given coordinates on the topmost layer.
        * Updates the brush stroke result and sets the dirty flag.
        *
        * @param x X coordinate of brush center
        * @param y Y coordinate of brush center
        * @param brush The brush object containing stroke parameters
        */
        void ApplyBrush(int x, int y, const Brush& brush);

        /**
        * Check if document has unsaved changes.
        *
        * Returns the dirty flag which indicates whether the document
        * has been modified since the last save operation.
        *
        * @return true if document has unsaved changes, false otherwise
        */
        [[nodiscard]] bool IsDirty() const;

    private:
        std::filesystem::path m_filePath;
        ColorSpace m_colorSpace = ColorSpace::RGB;
        bool m_isDirty = false;

        // Maybe unneccesary
        //int m_width;
        //int m_height;

        std::vector<Layer> m_layers{};
        std::stack<Layer> m_undoStack{};
    };
}
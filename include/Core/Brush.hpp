/*
 * Project: ImageEditor
 * File: Bruch.hpp
 * Author: olegfresi
 * Created: 03/03/26 11:55
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
#include <vector>
#include <string>
#include "Image.hpp"
#include "Color.hpp"

namespace Editor
{
    /**
    * Brush tool for painting on images.
    *
    * Represents a brush with customizable size, opacity, and name.
    * Can be applied to images at specified coordinates with a given color.
    * Supports variable opacity for semi-transparent strokes.
    */
    class Brush
    {
    public:
        /**
        * Constructor to create a brush with parameters.
        *
        * Initializes a new brush with the specified name, size, and opacity.
        *
        * @param name Name identifier for the brush (e.g., "Round", "Soft")
        * @param size Brush size in pixels (diameter)
        * @param opacity Brush opacity in range [0.0, 1.0] where 0 is transparent and 1 is opaque
        */
        Brush(std::string name, int size, float opacity)
            : m_Name(std::move(name)), m_Size(size), m_Opacity(opacity) {}

        /**
        * Apply brush stroke to an image.
        *
        * Paints the brush at the specified coordinates on the image with the given color.
        * The brush shape, size, and opacity determine the painted area and intensity.
        * Image is modified in-place.
        *
        * @param image Image to paint on (modified in-place)
        * @param x X coordinate (column) for brush center
        * @param y Y coordinate (row) for brush center
        * @param color Color to paint with (RGBA)
        */
        void Apply(Image& image, int x, int y, const Color& color);

        /**
        * Get brush size.
        *
        * @return Size in pixels (diameter)
        */
        [[nodiscard]] int GetSize() const { return m_Size; }

        /**
        * Get brush opacity.
        *
        * @return Opacity value in range [0.0, 1.0]
        */
        [[nodiscard]] float GetOpacity() const { return m_Opacity; }

        /**
        * Get brush name.
        *
        * @return Reference to the brush name string
        */
        [[nodiscard]] const std::string& GetName() const { return m_Name; }

        /**
        * Set brush size.
        *
        * @param size New size in pixels (diameter)
        */
        void SetSize(int size) { m_Size = size; }

        /**
        * Set brush opacity.
        *
        * @param opacity New opacity in range [0.0, 1.0]
        */
        void SetOpacity(float opacity) { m_Opacity = opacity; }

    private:
        std::string m_Name;      ///< Brush name/identifier
        int m_Size;              ///< Brush size in pixels
        float m_Opacity;         ///< Brush opacity [0.0, 1.0]
    };

    /**
    * Manager for multiple brushes.
    *
    * Maintains a collection of brushes and tracks the currently active brush.
    * Allows switching between different brush presets during editing.
    */
    class BrushManager
    {
    public:
        /**
        * Add a brush to the collection.
        *
        * Registers a new brush in the brush collection.
        * Brushes can then be retrieved by name.
        *
        * @param brush The brush object to add
        */
        void AddBrush(const Brush& brush);

        /**
        * Set the current active brush.
        *
        * Changes which brush is currently selected for use.
        * The brush name must exist in the collection.
        *
        * @param name Name of the brush to activate
        */
        void SetCurrentBrush(const std::string& name);

        /**
        * Get a brush by name.
        *
        * Retrieves a brush from the collection by its name identifier.
        *
        * @param name Name of the brush to retrieve
        * @return Const reference to the requested brush
        */
        [[nodiscard]] const Brush& GetBrush(const std::string& name) const;

        /**
        * Get the currently active brush.
        *
        * Returns the brush that is currently selected for use.
        *
        * @return Const reference to the current active brush
        */
        [[nodiscard]] const Brush& GetCurrentBrush() const;

    private:
        std::vector<Brush> m_Brushes;
        std::string m_CurrentBrushName;
    };
}
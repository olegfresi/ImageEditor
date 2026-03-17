/*
 * Project: ImageEditor
 * File: GrayScaleCommand.hpp
 * Author: olegfresi
 * Created: 14/03/26 20:59
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
#include "Command.hpp"
#include "../Core/Document.hpp"
#include "../Core/Processing.hpp"


namespace Editor::Command
{
    class GrayScaleCommand : public ICommand
    {
    public:
        GrayScaleCommand(Document* document) : m_document{document} {}
        ~GrayScaleCommand() override = default;

        void Execute() override
        {
            auto& image = m_document->GetImage();
            auto pixels = image.GetPixelData();

            m_backup.assign(pixels.begin(), pixels.end());

            Processor::ToGrayScale(image);
        }
        void Undo() override
        {
            auto& image = m_document->GetImage();
            auto pixels = image.GetPixelData();

            std::ranges::copy(m_backup.begin(), m_backup.end(), pixels.begin());
        }

    private :
        Document* m_document;
        std::vector<Pixel> m_backup;
    };
}
/*
 * Project: ImageEditor
 * File: SharpenCommand.hpp
 * Author: olegfresi
 * Created: 14/03/26 21:00
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
#include "../Core/Document.hpp"
#include "Command.hpp"


namespace Editor::Command
{
    class SharpenCommand : public ICommand
    {
    public:
        SharpenCommand(Document* doc) : m_document{doc} {}
        ~SharpenCommand() override = default;

        void Execute() override
        {
            auto& img = m_document->GetImage();
            auto pixels = img.GetPixelData();

            m_backup.assign(pixels.begin(), pixels.end());

            Processor::Invert(img);
        }

        void Undo() override
        {
            auto pixels = m_document->GetImage().GetPixelData();
            std::ranges::copy(m_backup.begin(), m_backup.end(), pixels.begin());
        }

    private:
        Document* m_document;
        std::vector<Pixel> m_backup;
    };
}
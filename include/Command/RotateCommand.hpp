/*
 * Project: ImageEditor
 * File: RotateCommand.hpp
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
#include "../Core/Document.hpp"
#include "Command.hpp"
#include <iostream>


namespace Editor::Command
{
    class RotateCommand : public ICommand
    {
    public:
        RotateCommand(Document* doc, float angle) : m_document{doc}, m_angle{angle} {}
        ~RotateCommand() override = default;

        void Execute() override
        {
            auto start = std::chrono::high_resolution_clock::now();

            Processor::Rotate(m_document->GetImage(), m_angle);

            auto end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            std::cout << "Rotate time: " << duration.count() << " ms" << std::endl;
        }

        void Undo() override
        {
            Processor::Rotate(m_document->GetImage(), -m_angle);
        }

    private:
        Document* m_document;
        float m_angle;
    };
}
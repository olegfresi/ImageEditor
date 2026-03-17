/*
 * Project: ImageEditor
 * File: ToneCurveChangeCommand.hpp
 * Author: olegfresi
 * Created: 15/03/26 22:27
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


namespace Editor::Command
{
    using namespace  Widget;
    class ToneCurveCommand : public ICommand
    {
    public:
        ToneCurveCommand(Document* doc, ToneCurveWidget& widget, const ToneCurveState& before,
                         const ToneCurveState& after, const std::vector<Pixel>& beforePixels)
            : m_document{doc}, m_widget{widget}, m_before{before}, m_after{after}, m_beforePixels{beforePixels} {}

        void Execute() override
        {
            Apply(m_after);
        }

        void Undo() override
        {
            auto& img = m_document->GetImage();

            std::ranges::copy(m_beforePixels.begin(), m_beforePixels.end(), img.GetPixelData().begin());

            m_widget.SetState(m_before);
        }

    private:
        void Apply(const ToneCurveState& state)
        {
            auto& img = m_document->GetImage();

            Utils::ApplyLut(img.GetPixelData(), m_beforePixels, state.lut);

            m_widget.SetState(state);
        }

    private:
        Document* m_document;
        ToneCurveWidget& m_widget;

        ToneCurveState m_before;
        ToneCurveState m_after;

        std::vector<Pixel> m_beforePixels;
    };
}
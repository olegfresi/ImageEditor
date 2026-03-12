#include "../../include/Core/Document.hpp"


namespace Editor
{
    Document::Document() = default;
    Document::Document(std::filesystem::path filePath) : m_filePath(std::move(filePath)){}
    Document::Document(const File& file) : m_filePath{file.GetPath()} {}

    void Document::Save()
    {

    }

    void Document::SaveAs(const std::filesystem::path &path)
    {
    }

    void Document::AddLayer()
    {
    }

    void Document::RemoveLayer(int index)
    {
    }

    void Document::ApplyBrush(int x, int y, const Brush& brush)
    {
    }

    bool Document::IsDirty() const
    {
        return m_isDirty;
    }
}

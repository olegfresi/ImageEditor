#include "../../include/Core/Document.hpp"
#include "../../include/Core/FileManager.hpp"


namespace Editor
{

    Document::Document(int width, int height, Image image, ColorSpace color) : m_width{width}, m_height{height},
        m_revision{1}, m_colorSpace{color}, m_image{std::move(image)} {}

    bool Document::IsDirty() const { return m_isDirty; }

    void Document::ExecuteCommand(std::unique_ptr<Command> command)
    {
        command->Execute();
        m_undoStack.push(std::move(command));
        m_isDirty = true;
    }

    void Document::Undo()
    {
        m_undoStack.pop();
    }

    void Document::Redo()
    {
        m_redoStack.pop();
    }

    void Document::Save()
    {
        if (m_filePath.empty())
            throw std::runtime_error("No file path set. Use SaveAs() to specify a file.");

        FileManager::SaveAs(*this, m_filePath);

        m_isDirty = false;
    }

    void Document::SaveAs(const std::filesystem::path& path)
    {
        if (!FileManager::SaveAs(*this, path))
            throw std::runtime_error("Failed to save document to " + path.string());

        m_filePath = path;

        m_isDirty = false;
    }
}

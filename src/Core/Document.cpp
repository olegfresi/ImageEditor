#include "../../include/Core/Document.hpp"

#include <iostream>

#include "../../include/Core/FileManager.hpp"


namespace Editor
{

    Document::Document(int width, int height, Image image, ColorSpace color) : m_width{width}, m_height{height},
        m_revision{1}, m_colorSpace{color}, m_image{std::move(image)} {}

    bool Document::IsDirty() const { return m_isDirty; }

    void Document::ExecuteCommand(std::unique_ptr<Command::ICommand> command)
    {
        command->Execute();

        m_undoStack.push_back(std::move(command));
        m_redoStack.clear();
        m_isDirty = true;

        NotifyImageChanged();
        UpdateDirtyFlag();

        if(m_onCommandStackChanged)
            m_onCommandStackChanged();
    }

    void Document::Undo()
    {
        if(m_undoStack.empty())
            return;

        auto cmd = std::move(m_undoStack.back());
        m_undoStack.pop_back();

        cmd->Undo();

        m_redoStack.push_back(std::move(cmd));

        UpdateDirtyFlag();

        NotifyImageChanged();

        if(m_onCommandStackChanged)
            m_onCommandStackChanged();
    }

    void Document::Redo()
    {
        if(m_redoStack.empty())
            return;

        auto cmd = std::move(m_redoStack.back());
        m_redoStack.pop_back();

        cmd->Execute();

        m_undoStack.push_back(std::move(cmd));

        UpdateDirtyFlag();

        NotifyImageChanged();

        if(m_onCommandStackChanged)
            m_onCommandStackChanged();
    }

    void Document::Save()
    {
        if (m_filePath.empty())
            throw std::runtime_error("No file path set. Use SaveAs() to specify a file.");

        FileManager::SaveAs(*this, m_filePath);

        m_saveIndex = m_undoStack.size();
        m_isDirty = false;
    }

    void Document::SaveAs(const std::filesystem::path& path)
    {
        if (!FileManager::SaveAs(*this, path))
            throw std::runtime_error("Failed to save document to " + path.string());

        m_filePath = path;

        m_isDirty = false;
    }

    void Document::UpdateDirtyFlag()
    {
        m_isDirty = (m_undoStack.size() != m_saveIndex);
    }

    void Document::NotifyImageChanged() const
    {
        if(m_onImageChanged)
            m_onImageChanged();
    }
}

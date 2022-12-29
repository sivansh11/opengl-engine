#ifndef CONTENT_BROWSER_PANEL_HPP
#define CONTENT_BROWSER_PANEL_HPP

#include "panel.hpp"

#include "gfx/texture.hpp"

#include <iostream>
#include <filesystem>

class ContentBrowserPanel : public Panel {
public:
    ContentBrowserPanel(std::filesystem::path initialPath);

    void renderPanel() override;

private:
    std::filesystem::path m_path;
    gfx::Texture2D m_directoryIcon;
    gfx::Texture2D m_fileIcon;
};

#endif
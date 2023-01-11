#ifndef CONTENT_BROWSER_PANEL_HPP
#define CONTENT_BROWSER_PANEL_HPP

#include "core/panel.hpp"

#include "gfx/texture.hpp"

#include <iostream>
#include <filesystem>
#include <memory>

class ContentBrowserPanel : public core::Panel {
public:
    ContentBrowserPanel(std::filesystem::path initialPath);

    void onImGuiRender() override;

private:
    std::filesystem::path m_path;
    std::shared_ptr<gfx::Texture> m_directoryIcon;
    std::shared_ptr<gfx::Texture> m_fileIcon;
};

#endif
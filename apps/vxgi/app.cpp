#include "app.hpp"

#include "renderer/model.hpp"

#include "gfx/shaders.hpp"

#include "custom_events.hpp"

#include "core/imgui_utils.hpp"

#include "editor_camera.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define ASSETS_PATH std::string("../../../assets")

App::App() : window("Editor", 1600, 800) {
    core::initImgui(window);
    core::setupImGuiStyle();
    core::myDefaultImGuiStyle();
}

App::~App() {
    core::terminateImgui();
}

void App::run() {
    EditorCamera camera;

    gfx::ShaderProgram voxelizationPass{};
    voxelizationPass.addShader(ASSETS_PATH + "/shaders/voxels/voxelize.vert");
    voxelizationPass.addShader(ASSETS_PATH + "/shaders/voxels/voxelize.geom");
    voxelizationPass.addShader(ASSETS_PATH + "/shaders/voxels/voxelize.frag");
    voxelizationPass.link();

    gfx::ShaderProgram VCT{};
    VCT.addShader(ASSETS_PATH + "/shaders/voxels/voxel_cone_tracing.vert");
    VCT.addShader(ASSETS_PATH + "/shaders/voxels/voxel_cone_tracing.frag");
    VCT.link();

    renderer::Model model{ASSETS_PATH + "/models/2.0/Sponza/glTF/Sponza.gltf"};

    const uint32_t voxelGridSize = 128;

    auto voxels = gfx::Texture::Builder().build(gfx::Texture::Type::e3D);
    gfx::Texture::CreateInfo createInfo{};
    createInfo.width = voxelGridSize;
    createInfo.height = voxelGridSize;
    createInfo.depth = voxelGridSize;
    createInfo.internalFormat = gfx::Texture::InternalFormat::eRGBA8;
    createInfo.genMipMap = true;
    voxels->create(createInfo);

    uint8_t *data = new uint8_t[voxelGridSize * voxelGridSize * voxelGridSize];
    // for (int i = 0; i < voxelGridSize; i++) {
    //     for (int j = 0; j < voxelGridSize; j++) {
    //         for (int k = 0; k < voxelGridSize; k++) {
    //         }
    //     }
    // }

    gfx::Texture::UploadInfo uploadInfo{};
    uploadInfo.data = data;
    uploadInfo.dataType = gfx::Texture::DataType::eUNSIGNED_BYTE;
    uploadInfo.format = gfx::Texture::Format::eRGBA;
    uploadInfo.level = 0;
    uploadInfo.xoffset = 0;
    uploadInfo.yoffset = 0;
    uploadInfo.zoffset = 0;
    voxels->upload(uploadInfo);

    ImVec2 size{100, 100};

    double lastTime = glfwGetTime();

    while (!window.shouldClose()) {
        window.pollEvents();

        double currentTime = glfwGetTime(); 
        if (!(currentTime - lastTime >= 1.0 / 60.0f)) continue;
        float dt = currentTime - lastTime;
        lastTime = currentTime;

        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "voxelization pass");
        glViewport(0, 0, voxelGridSize, voxelGridSize);
        voxelizationPass.bind();
        glm::mat4 view = camera.getView();
        glm::mat4 projection = camera.getProjection();
        glm::vec3 color{1, 1, 1};
        glm::vec3 position{0, 0, 0};
        voxelizationPass.mat4f("view", glm::value_ptr(view));
        voxelizationPass.mat4f("projection", glm::value_ptr(projection));
        voxelizationPass.vec3f("pointLights.color", glm::value_ptr(color));
        voxelizationPass.vec3f("pointLights.position", glm::value_ptr(position));
        voxelizationPass.veci("numberOfLights", 1);
        voxels->bindImage("voxels", 5, voxelizationPass);
        model.draw(voxelizationPass, {});
        voxels->genMipMaps();
        glPopDebugGroup();

        glClear(GL_COLOR_BUFFER_BIT);
        
        core::startFrameImgui();
        
        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None & ~ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | 
                                       ImGuiWindowFlags_NoTitleBar |
                                       ImGuiWindowFlags_NoCollapse |
                                       ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus |
                                       ImGuiWindowFlags_NoNavFocus |
                                       ImGuiWindowFlags_NoBackground |
                                       ImGuiWindowFlags_NoDecoration;
                                       
        bool dockSpace = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        auto mainViewPort = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(mainViewPort->WorkPos);
        ImGui::SetNextWindowSize(mainViewPort->WorkSize);
        ImGui::SetNextWindowViewport(mainViewPort->ID);
        ImGui::Begin("DockSpace", &dockSpace, windowFlags);
        ImGuiID dockspaceID = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockspaceID, ImGui::GetContentRegionAvail(), dockspaceFlags);
        // if (ImGui::BeginMainMenuBar()) {
        //     if (ImGui::BeginMenu("Panels")) {
        //         for (auto& panel : panels) {
        //             if (ImGui::MenuItem(panel->getName().c_str(), NULL, &panel->getShow())) {}
        //         }
        //         ImGui::EndMenu();
        //     }
        //     ImGui::EndMainMenuBar();
        // }
        ImGui::End();
        ImGui::PopStyleVar(2);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
        ImGui::SetNextWindowClass(&window_class);
        ImGuiWindowFlags viewPortFlags = ImGuiWindowFlags_NoTitleBar |
                                         ImGuiWindowFlags_NoDecoration;
        ImGui::Begin("viewport", nullptr, viewPortFlags);
        ImVec2 vpSize = ImGui::GetContentRegionAvail();
        if (vpSize.x != size.x || vpSize.y != size.y) {
            size = vpSize;
            window.getDispatcher().post<ViewPortResizeEvent>(static_cast<uint32_t>(vpSize.x), static_cast<uint32_t>(vpSize.y));
        }
        camera.onUpdate(dt);
        // ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void*>(renderer.getRenderedImage())), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
        
        ImGui::End();
        ImGui::PopStyleVar(2);

        core::endFrameImgui(window);

        window.updateScreen();
    }
}
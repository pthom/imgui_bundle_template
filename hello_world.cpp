// Part of ImGui Bundle - MIT License - Copyright (c) 2022-2026 Pascal Thomet - https://github.com/pthom/imgui_bundle
//
// Example application using many libraries of Dear ImGui Bundle:
// ImmApp / HelloImGui, markdown (with LaTeX), ImPlot, ImPlot3D, ImmVision, imgui-knobs, imgui_toggle, imspinner,
// ImAnim, NanoVG, imgui-node-editor, ImGuizmo, imgui_tex_inspect, ImGui Test Engine.
//
// The IMGUI_BUNDLE_WITH_* macros are provided by imgui_bundle, according to how it was built:
// they let this example compile even if some optional libraries were disabled.
#include "immapp/immapp.h"
#include "imgui_md_wrapper/imgui_md_wrapper.h"
#include "imgui-knobs/imgui-knobs.h"
#include "imgui_toggle/imgui_toggle.h"
#include "imspinner/imspinner.h"
#ifdef IMGUI_BUNDLE_WITH_IMPLOT
#include "implot/implot.h"
#endif
#ifdef IMGUI_BUNDLE_WITH_IMPLOT3D
#include "implot3d/implot3d.h"
#endif
#ifdef IMGUI_BUNDLE_WITH_IMMVISION
#include "immvision/immvision.h"
#endif
#ifdef IMGUI_BUNDLE_WITH_IMANIM
#include "ImAnim/im_anim.h"
#endif
#ifdef IMGUI_BUNDLE_WITH_NANOVG
#include "nanovg.h"
#include "nvg_imgui/nvg_imgui.h"
#endif
#ifdef IMGUI_BUNDLE_WITH_IMGUI_NODE_EDITOR
#include "imgui-node-editor/imgui_node_editor.h"
#endif
#ifdef IMGUI_BUNDLE_WITH_IMGUIZMO
#include "ImGuizmo.h"
#endif
#ifdef IMGUI_BUNDLE_WITH_IMGUI_TEX_INSPECT
#include "imgui_tex_inspect/imgui_tex_inspect.h"
#endif
#ifdef HELLOIMGUI_WITH_TEST_ENGINE
#include "imgui_test_engine/imgui_te_engine.h"
#include "imgui_test_engine/imgui_te_context.h"
#include "imgui_test_engine/imgui_te_ui.h"
#endif

#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>


void GuiMarkdown()
{
    ImGuiMd::RenderUnindented(R"(
        # Dear ImGui Bundle
        [Dear ImGui Bundle](https://github.com/pthom/imgui_bundle) is a bundle for [Dear ImGui](https://github.com/ocornut/imgui.git),
        including various useful libraries from its ecosystem.
        It enables to easily create ImGui applications in C++, as well as in Python.

        This is an example of markdown widget, with an included image (from the `assets/` folder):

        ![world](images/world.png)

        *Note: the icon of this application is defined by `assets/app_settings/icon.png`*
    )");

#ifdef IMGUI_RICHMD_WITH_LATEX
    ImGuiMd::RenderUnindented(R"(
        ---
        ## LaTeX
        Math is rendered natively by *MicroTeX*: inline $E = mc^2$ and $\sqrt{a^2 + b^2}$, or display math:

        $$x = \frac{-b \pm \sqrt{b^2 - 4ac}}{2a}$$
    )");
#endif
}


void GuiPlots()
{
    static float xs[200], ys[200], zs[200];
    for (int i = 0; i < 200; ++i)
    {
        float t = i * 0.05f;
        xs[i] = t; ys[i] = sinf(t); zs[i] = cosf(t);
    }
#ifdef IMGUI_BUNDLE_WITH_IMPLOT
    if (ImPlot::BeginPlot("ImPlot", ImVec2(-1, 250)))
    {
        ImPlot::PlotLine("sin", xs, ys, 200);
        ImPlot::PlotLine("cos", xs, zs, 200);
        ImPlot::EndPlot();
    }
#endif
#ifdef IMGUI_BUNDLE_WITH_IMPLOT3D
    if (ImPlot3D::BeginPlot("ImPlot3D", ImVec2(-1, 300)))
    {
        ImPlot3D::PlotLine("helix", ys, zs, xs, 200);
        ImPlot3D::EndPlot();
    }
#endif
}


#ifdef IMGUI_BUNDLE_WITH_IMMVISION
void GuiImmVision()
{
    // A generated RGB image: ImmVision::ImageBuffer wraps raw pixels (no OpenCV needed)
    static const int w = 256, h = 256;
    static std::vector<uint8_t> pixels = [] {
        std::vector<uint8_t> p(w * h * 3);
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
            {
                uint8_t* px = &p[(y * w + x) * 3];
                px[0] = (uint8_t)x; px[1] = (uint8_t)y; px[2] = 128;
            }
        return p;
    }();
    ImmVision::ImageBuffer image(pixels.data(), w, h, 3);

    ImGui::Text("ImmVision::ImageDisplay (simple)");
    ImmVision::ImageDisplay("gradient", image, ImmVision::Size(w / 2, h / 2));

    ImGui::Text("ImmVision::Image (zoom, pan, pixel inspection)");
    static ImmVision::ImageParams params;
    params.ImageDisplaySize = ImmVision::Size(300, 300);
    ImmVision::Image("gradient_viewer", image, &params);
}
#endif


void GuiWidgets()
{
    static float gain = 3.f;
    ImGuiKnobs::Knob("Gain", &gain, 0.f, 10.f);
    ImGui::SameLine();
    static bool enabled = true;
    ImGui::Toggle("Enabled", &enabled);
    ImGui::SameLine();
    ImSpinner::SpinnerAng("spinner", 16.f, 3.f);

#ifdef IMGUI_BUNDLE_WITH_IMANIM
    // ImAnim: animate a value towards a target (the "Enabled" toggle above)
    ImGui::SeparatorText("ImAnim");
    float animated = iam_tween_float(
        ImGui::GetID("anim"), 0, enabled ? 1.f : 0.f, 0.8f,
        iam_ease_back(1.5f), iam_policy_crossfade, ImGui::GetIO().DeltaTime);
    ImGui::ProgressBar(animated, ImVec2(-1, 0), "toggle \"Enabled\" to animate");
#endif
}


#ifdef IMGUI_BUNDLE_WITH_NANOVG
// NanoVG: vector drawing into a framebuffer, displayed as an image.
// The NanoVG context requires an active render backend: it is created in PostInit and released in BeforeExit.
struct NanoVgState
{
    NVGcontext* vg = nullptr;
    std::unique_ptr<NvgImgui::NvgFramebuffer> framebuffer;

    void Init()
    {
        vg = NvgImgui::CreateNvgContext_HelloImGui(NvgImgui::NVG_ANTIALIAS | NvgImgui::NVG_STENCIL_STROKES);
        framebuffer = std::make_unique<NvgImgui::NvgFramebuffer>(vg, 512, 256, 0);
    }
    void Release()
    {
        framebuffer.reset();
        NvgImgui::DeleteNvgContext_HelloImGui(vg);
        vg = nullptr;
    }
};
static NanoVgState gNanoVg;

void GuiNanoVg()
{
    auto draw = [](NVGcontext* vg, float width, float height) {
        nvgBeginPath(vg);
        nvgCircle(vg, width / 2.f, height / 2.f, height / 3.f);
        nvgFillColor(vg, nvgRGBA(255, 120, 50, 255));
        nvgFill(vg);
        nvgStrokeWidth(vg, 6.f);
        nvgStrokeColor(vg, nvgRGBA(255, 255, 255, 255));
        nvgStroke(vg);
    };
    NvgImgui::RenderNvgToFrameBuffer(gNanoVg.vg, *gNanoVg.framebuffer, draw, ImVec4(0.1f, 0.1f, 0.3f, 1.f));
    ImGui::Image(gNanoVg.framebuffer->TextureId, ImVec2(512, 256));
}
#endif


#ifdef IMGUI_BUNDLE_WITH_IMGUI_NODE_EDITOR
namespace ed = ax::NodeEditor;
void GuiNodeEditor()
{
    // ImmApp provides a default node editor context (AddOnsParams::withNodeEditor)
    ed::Begin("editor", ImVec2(0, 300));
    static bool first = true;
    if (first) ed::SetNodePosition(1, ImVec2(20, 40));
    ed::BeginNode(1);
    ImGui::Text("Node A");
    ed::BeginPin(2, ed::PinKind::Output); ImGui::Text("out ->"); ed::EndPin();
    ed::EndNode();

    if (first) ed::SetNodePosition(3, ImVec2(250, 40));
    ed::BeginNode(3);
    ImGui::Text("Node B");
    ed::BeginPin(4, ed::PinKind::Input); ImGui::Text("-> in"); ed::EndPin();
    ed::EndNode();

    ed::Link(5, 2, 4);
    ed::End();
    first = false;
}
#endif


#ifdef IMGUI_BUNDLE_WITH_IMGUIZMO
void GuiImGuizmo()
{
    static float view[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size(256, 256);
    ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);
    ImGuizmo::ViewManipulate(view, 8.f, pos, size, 0x10101010);
    ImGui::Dummy(size);
    ImGui::Text("Drag the cube to rotate the view");
}
#endif


#ifdef IMGUI_BUNDLE_WITH_IMGUI_TEX_INSPECT
void GuiTexInspect()
{
    static ImTextureID textureId = HelloImGui::ImTextureIdFromAsset("images/world.png");
    static ImVec2 textureSize = HelloImGui::ImageSizeFromAsset("images/world.png");
    ImGuiTexInspect::BeginInspectorPanel("Inspector", textureId, textureSize, 0,
                                         ImGuiTexInspect::SizeIncludingBorder(ImVec2(500, 350)));
    ImGuiTexInspect::EndInspectorPanel();  // must be called even if BeginInspectorPanel returned false
}
#endif


#ifdef HELLOIMGUI_WITH_TEST_ENGINE
static ImGuiTest* gTestVisitTabs = nullptr;

void RegisterTests()
{
    ImGuiTestEngine* engine = HelloImGui::GetImGuiTestEngine();
    gTestVisitTabs = IM_REGISTER_TEST(engine, "Example", "Visit all tabs");
    gTestVisitTabs->TestFunc = [](ImGuiTestContext* ctx) {
        // Target the app's main window explicitly (HelloImGui's default full screen window):
        // "//$FOCUSED" would point to the Test Engine window when the test is started from there.
        ctx->SetRef("Main window (title bar invisible)");
        for (const char* tab : { "Plots", "Widgets", "Markdown", "Test Engine" })
            ctx->ItemClick((std::string("tabs/") + tab).c_str());
    };
}

void GuiTestEngine()
{
    ImGuiTestEngine* engine = HelloImGui::GetImGuiTestEngine();
    if (ImGui::Button("Run \"Visit all tabs\""))
        ImGuiTestEngine_QueueTest(engine, gTestVisitTabs);
    ImGuiTestEngine_ShowTestEngineWindows(engine, nullptr);
}
#endif


void Gui()
{
    if (ImGui::BeginTabBar("tabs"))
    {
        if (ImGui::BeginTabItem("Markdown")) { GuiMarkdown(); ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Plots")) { GuiPlots(); ImGui::EndTabItem(); }
#ifdef IMGUI_BUNDLE_WITH_IMMVISION
        if (ImGui::BeginTabItem("ImmVision")) { GuiImmVision(); ImGui::EndTabItem(); }
#endif
        if (ImGui::BeginTabItem("Widgets")) { GuiWidgets(); ImGui::EndTabItem(); }
#ifdef IMGUI_BUNDLE_WITH_NANOVG
        if (ImGui::BeginTabItem("NanoVG")) { GuiNanoVg(); ImGui::EndTabItem(); }
#endif
#ifdef IMGUI_BUNDLE_WITH_IMGUI_NODE_EDITOR
        if (ImGui::BeginTabItem("Node Editor")) { GuiNodeEditor(); ImGui::EndTabItem(); }
#endif
#ifdef IMGUI_BUNDLE_WITH_IMGUIZMO
        if (ImGui::BeginTabItem("ImGuizmo")) { GuiImGuizmo(); ImGui::EndTabItem(); }
#endif
#ifdef IMGUI_BUNDLE_WITH_IMGUI_TEX_INSPECT
        if (ImGui::BeginTabItem("Tex Inspect")) { GuiTexInspect(); ImGui::EndTabItem(); }
#endif
#ifdef HELLOIMGUI_WITH_TEST_ENGINE
        if (ImGui::BeginTabItem("Test Engine")) { GuiTestEngine(); ImGui::EndTabItem(); }
#endif
        ImGui::EndTabBar();
    }
}


int main(int , char *[])
{
#ifdef ASSETS_LOCATION
    HelloImGui::SetAssetsFolder(ASSETS_LOCATION);
#endif
#ifdef IMGUI_BUNDLE_WITH_IMMVISION
    ImmVision::UseRgbColorOrder();
#endif

    HelloImGui::RunnerParams runnerParams;
    runnerParams.appWindowParams.windowTitle = "Dear ImGui Bundle example";
    runnerParams.appWindowParams.windowGeometry.size = {900, 800};
    runnerParams.callbacks.ShowGui = Gui;
#ifdef IMGUI_BUNDLE_WITH_NANOVG
    runnerParams.callbacks.EnqueuePostInit([] { gNanoVg.Init(); });
    runnerParams.callbacks.EnqueueBeforeExit([] { gNanoVg.Release(); });
#endif
#ifdef HELLOIMGUI_WITH_TEST_ENGINE
    runnerParams.useImGuiTestEngine = true;
    runnerParams.callbacks.RegisterTests = RegisterTests;
#endif

    ImmApp::AddOnsParams addOnsParams;
    addOnsParams.withMarkdown = true;
#ifdef IMGUI_RICHMD_WITH_LATEX
    addOnsParams.withLatex = true;
#endif
#ifdef IMGUI_BUNDLE_WITH_IMPLOT
    addOnsParams.withImplot = true;
#endif
#ifdef IMGUI_BUNDLE_WITH_IMPLOT3D
    addOnsParams.withImplot3d = true;
#endif
#ifdef IMGUI_BUNDLE_WITH_IMGUI_NODE_EDITOR
    addOnsParams.withNodeEditor = true;
#endif
#ifdef IMGUI_BUNDLE_WITH_IMGUI_TEX_INSPECT
    addOnsParams.withTexInspect = true;
#endif
#ifdef IMGUI_BUNDLE_WITH_IMANIM
    addOnsParams.withImAnim = true;
#endif

    ImmApp::Run(runnerParams, addOnsParams);
    return 0;
}

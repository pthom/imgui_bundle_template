// Part of ImGui Bundle - MIT License - Copyright (c) 2022-2026 Pascal Thomet - https://github.com/pthom/imgui_bundle
//
// Example application using several libraries of Dear ImGui Bundle:
// ImmApp / HelloImGui, markdown (with LaTeX), ImPlot, ImPlot3D, ImmVision, imgui-knobs, imgui_toggle, imspinner.
//
// The IMGUI_BUNDLE_WITH_* macros are provided by imgui_bundle, according to how it was built:
// they let this example compile even if some optional libraries were disabled.
#include "immapp/immapp.h"
#include "imgui_md_wrapper/imgui_md_wrapper.h"
#ifdef IMGUI_BUNDLE_WITH_IMPLOT
#include "implot/implot.h"
#endif
#ifdef IMGUI_BUNDLE_WITH_IMPLOT3D
#include "implot3d/implot3d.h"
#endif
#ifdef IMGUI_BUNDLE_WITH_IMMVISION
#include "immvision/immvision.h"
#endif
#include "imgui-knobs/imgui-knobs.h"
#include "imgui_toggle/imgui_toggle.h"
#include "imspinner/imspinner.h"

#include <cmath>
#include <cstdint>
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
}


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

    HelloImGui::SimpleRunnerParams runnerParams;
    runnerParams.guiFunction = Gui;
    runnerParams.windowTitle = "Dear ImGui Bundle example";
    runnerParams.windowSize = {800, 800};

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

    ImmApp::Run(runnerParams, addOnsParams);
    return 0;
}

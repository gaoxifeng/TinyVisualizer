#include "ImGuiPlugin.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui/imgui_fonts_droid_sans.h"
#include <include/GLFW/glfw3.h>
#include <iostream>

namespace DRAWER {
	void ImGuiPlugin::init(GLFWwindow* window) {
		// Setup ImGui binding
		if (window) {
			IMGUI_CHECKVERSION();
			if (!context_) {
				// Single global context by default, but can be overridden by the user
				static ImGuiContext* __global_context = ImGui::CreateContext();
				context_ = __global_context;
			}
			const char* glsl_version = "#version 150";
			ImGui_ImplGlfw_InitForOpenGL(window, false);
			ImGui_ImplOpenGL3_Init(glsl_version);
			ImGui::GetIO().IniFilename = nullptr;
			ImGui::StyleColorsDark();
			ImGuiStyle& style = ImGui::GetStyle();
			style.FrameRounding = 5.0f;
			reloadFont();
		}
	}
	void ImGuiPlugin::reloadFont(int font_size) {
		_hidpiScaling = hidpiScaling();
		pixel_ratio_ = pixelRatio();
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Clear();
		io.Fonts->AddFontFromMemoryCompressedTTF(droid_sans_compressed_data,
			droid_sans_compressed_size, font_size * _hidpiScaling);
		io.FontGlobalScale = 1.0 / pixel_ratio_;
	}

	void ImGuiPlugin::clear() {
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		// User is responsible for destroying context if a custom context is given
		// ImGui::DestroyContext(*context_);
	}

	void ImGuiPlugin::preDraw() {
		glfwPollEvents();

		// Check whether window dpi has changed
		float scaling = hidpiScaling();
		if (std::abs(scaling - _hidpiScaling) > 1e-5) {
			reloadFont();
			ImGui_ImplOpenGL3_DestroyDeviceObjects();
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiPlugin::postDraw() {
		drawMenu();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	// Draw menu
	void ImGuiPlugin::drawMenu() {
		// Viewer settings
		drawViewerWindow();
	}

	void ImGuiPlugin::drawViewerWindow() {
		float menuWidth = 180.f * menuScaling();
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints(ImVec2(menuWidth, -1.0f), ImVec2(menuWidth, -1.0f));
		bool _viewer_menu_visible = true;
		ImGui::Begin(
			"Viewer", &_viewer_menu_visible,
			ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_AlwaysAutoResize
		);
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);
		drawViewerMenu();
		ImGui::PopItemWidth();
		ImGui::End();
	}

	void ImGuiPlugin::drawViewerMenu() {
	}

	float ImGuiPlugin::pixelRatio() {
		// Computes pixel ratio for hidpi devices
		int buf_size[2];
		int win_size[2];
		GLFWwindow* window = glfwGetCurrentContext();
		glfwGetFramebufferSize(window, &buf_size[0], &buf_size[1]);
		glfwGetWindowSize(window, &win_size[0], &win_size[1]);
		return (float)buf_size[0] / (float)win_size[0];
	}

	float ImGuiPlugin::hidpiScaling() {
		// Computes scaling factor for hidpi devices
		float xscale, yscale;
		GLFWwindow* window = glfwGetCurrentContext();
		glfwGetWindowContentScale(window, &xscale, &yscale);
		return 0.5 * (xscale + yscale);
	}
}

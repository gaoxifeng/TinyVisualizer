#ifndef IMGUIPlUGIN_H
#define IMGUIPlUGIN_H

#include "Drawer.h"

struct ImGuiContext;

namespace DRAWER {
	class ImGuiPlugin : public Plugin {

	protected:
		// Hidpi scaling to be used for text rendering.
		float _hidpiScaling;

		// Ratio between the framebuffer size and the window size.
		// May be different from the hipdi scaling!
		float pixel_ratio_;

		// ImGui Context
		ImGuiContext* context_ = nullptr;

	public:
		virtual void init(GLFWwindow* window);

		void reloadFont(int font_size = 13);

		virtual void clear() override;

		virtual void preDraw() override;

		virtual void postDraw() override;

		// Draw menu
		virtual void drawMenu();

		// Can be overwritten by `callback_draw_viewer_window`
		virtual void drawViewerWindow();

		// Can be overwritten by `callback_draw_viewer_menu`
		virtual void drawViewerMenu();

		float pixelRatio();

		float hidpiScaling();

		float menuScaling() { return _hidpiScaling / pixel_ratio_; }

	private:
	};
}
#endif

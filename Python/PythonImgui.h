#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include "imgui.h"
#include <vector>
#include <string>
#include <tuple>
#include <array>

void initImgui(py::module& mSuper) {
  auto m = mSuper.def_submodule("imgui");

  py::class_<ImVec2>(m,"ImVec2")
  .def(py::init<>())
  .def(py::init<float,float>())
  .def_readwrite("x",&ImVec2::x)
  .def_readwrite("y",&ImVec2::y);

  py::class_<ImVec4>(m,"ImVec4")
  .def(py::init<>())
  .def(py::init<float,float,float,float>())
  .def_readwrite("x",&ImVec4::x)
  .def_readwrite("y",&ImVec4::y)
  .def_readwrite("z",&ImVec4::z)
  .def_readwrite("w",&ImVec4::w);

  // Windows
  // - Begin() = push window to the stack and start appending to it. End() = pop window from the stack.
  // - Passing 'bool* p_open != NULL' shows a window-closing widget in the upper-right corner of the window,
  //   which clicking will set the boolean to false when clicked.
  // - You may append multiple times to the same window during the same frame by calling Begin()/End() pairs multiple times.
  //   Some information such as 'flags' or 'p_open' will only be considered by the first call to Begin().
  // - Begin() return false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting
  //   anything to the window. Always call a matching End() for each Begin() call, regardless of its return value!
  //   [Important: due to legacy reason, this is inconsistent with most other functions such as BeginMenu/EndMenu,
  //    BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function
  //    returned true. Begin and BeginChild are the only odd ones out. Will be fixed in a future update.]
  // - Note that the bottom of window stack always contains a window called "Debug".
  m.def("Begin",[](const char* name,ImGuiWindowFlags flags)->bool {
    return ImGui::Begin(name,NULL,flags);
  });
  m.def("Begin",[](const char* name,bool p_open,ImGuiWindowFlags flags)->std::tuple<bool,bool> {
    bool ret=ImGui::Begin(name,&p_open,flags);
    return std::make_tuple(ret,p_open);
  });
  m.def("End",&ImGui::End);

  // Child Windows
  // - Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window. Child windows can embed their own child.
  // - For each independent axis of 'size': ==0.0f: use remaining host window size / >0.0f: fixed size / <0.0f: use remaining window size minus abs(size) / Each axis can use a different mode, e.g. ImVec2(0,400).
  // - BeginChild() returns false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting anything to the window.
  //   Always call a matching EndChild() for each BeginChild() call, regardless of its return value.
  //   [Important: due to legacy reason, this is inconsistent with most other functions such as BeginMenu/EndMenu,
  //    BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function
  //    returned true. Begin and BeginChild are the only odd ones out. Will be fixed in a future update.]
  m.def("BeginChild",static_cast<bool(*)(const char*,const ImVec2&,bool,ImGuiWindowFlags)>(&ImGui::BeginChild));
  m.def("BeginChild",static_cast<bool(*)(ImGuiID,const ImVec2&,bool,ImGuiWindowFlags)>(&ImGui::BeginChild));
  m.def("EndChild",&ImGui::EndChild);

  // Windows Utilities
  // - 'current window' = the window we are appending into while inside a Begin()/End() block. 'next window' = next window we will Begin() into.
  m.def("IsWindowAppearing",&ImGui::IsWindowAppearing);
  m.def("IsWindowCollapsed",&ImGui::IsWindowCollapsed);
  m.def("IsWindowFocused",&ImGui::IsWindowFocused);
  m.def("IsWindowHovered",&ImGui::IsWindowHovered);
  m.def("GetWindowDrawList",&ImGui::GetWindowDrawList);
  m.def("GetWindowPos",&ImGui::GetWindowPos);
  m.def("GetWindowSize",&ImGui::GetWindowSize);
  m.def("GetWindowWidth",&ImGui::GetWindowWidth);
  m.def("GetWindowHeight",&ImGui::GetWindowHeight);

  // Window manipulation
  // - Prefer using SetNextXXX functions (before Begin) rather that SetXXX functions (after Begin).
  m.def("SetNextWindowPos",&ImGui::SetNextWindowPos);
  m.def("SetNextWindowSize",&ImGui::SetNextWindowSize);
  m.def("SetNextWindowSizeConstraints",&ImGui::SetNextWindowSizeConstraints);
  m.def("SetNextWindowContentSize",&ImGui::SetNextWindowContentSize);
  m.def("SetNextWindowCollapsed",&ImGui::SetNextWindowCollapsed);
  m.def("SetNextWindowFocus",&ImGui::SetNextWindowFocus);
  m.def("SetNextWindowBgAlpha",&ImGui::SetNextWindowBgAlpha);
  m.def("SetWindowPos",static_cast<void(*)(const ImVec2&,ImGuiCond)>(&ImGui::SetWindowPos));
  m.def("SetWindowSize",static_cast<void(*)(const ImVec2&,ImGuiCond)>(&ImGui::SetWindowSize));
  m.def("SetWindowCollapsed",static_cast<void(*)(bool,ImGuiCond)>(&ImGui::SetWindowCollapsed));
  m.def("SetWindowFocus",static_cast<void(*)()>(&ImGui::SetWindowFocus));
  m.def("SetWindowFontScale",&ImGui::SetWindowFontScale);
  m.def("SetWindowPos",static_cast<void(*)(const char*,const ImVec2&,ImGuiCond)>(&ImGui::SetWindowPos));
  m.def("SetWindowSize",static_cast<void(*)(const char*,const ImVec2&,ImGuiCond)>(&ImGui::SetWindowSize));
  m.def("SetWindowCollapsed",static_cast<void(*)(const char*,bool,ImGuiCond)>(&ImGui::SetWindowCollapsed));
  m.def("SetWindowFocus",static_cast<void(*)(const char*)>(&ImGui::SetWindowFocus));

  // Content region
  // - Retrieve available space from a given point. GetContentRegionAvail() is frequently useful.
  // - Those functions are bound to be redesigned (they are confusing, incomplete and the Min/Max return values are in local window coordinates which increases confusion)
  m.def("GetContentRegionAvail",&ImGui::GetContentRegionAvail);
  m.def("GetContentRegionMax",&ImGui::GetContentRegionMax);
  m.def("GetWindowContentRegionMin",&ImGui::GetWindowContentRegionMin);
  m.def("GetWindowContentRegionMax",&ImGui::GetWindowContentRegionMax);
  m.def("GetWindowContentRegionWidth",&ImGui::GetWindowContentRegionWidth);

  // Windows Scrolling
  m.def("GetScrollX",&ImGui::GetScrollX);
  m.def("GetScrollY",&ImGui::GetScrollY);
  m.def("SetScrollX",&ImGui::SetScrollX);
  m.def("SetScrollY",&ImGui::SetScrollY);
  m.def("GetScrollMaxX",&ImGui::GetScrollMaxX);
  m.def("GetScrollMaxY",&ImGui::GetScrollMaxY);
  m.def("SetScrollHereX",&ImGui::SetScrollHereX);
  m.def("SetScrollHereY",&ImGui::SetScrollHereY);
  m.def("SetScrollFromPosX",&ImGui::SetScrollFromPosX);
  m.def("SetScrollFromPosY",&ImGui::SetScrollFromPosY);

  // Parameters stacks (shared)
  m.def("PushFont",&ImGui::PushFont);
  m.def("PopFont",&ImGui::PopFont);
  m.def("PushStyleColor",static_cast<void(*)(ImGuiCol,ImU32)>(&ImGui::PushStyleColor));
  m.def("PushStyleColor",static_cast<void(*)(ImGuiCol,const ImVec4&)>(&ImGui::PushStyleColor));
  m.def("PopStyleColor",&ImGui::PopStyleColor);
  m.def("PushStyleVar",static_cast<void(*)(ImGuiStyleVar,float)>(&ImGui::PushStyleVar));
  m.def("PushStyleVar",static_cast<void(*)(ImGuiStyleVar,const ImVec2&)>(&ImGui::PushStyleVar));
  m.def("PopStyleVar",&ImGui::PopStyleVar);
  m.def("PushAllowKeyboardFocus",&ImGui::PushAllowKeyboardFocus);
  m.def("PopAllowKeyboardFocus",&ImGui::PopAllowKeyboardFocus);
  m.def("PushButtonRepeat",&ImGui::PushButtonRepeat);
  m.def("PopButtonRepeat",&ImGui::PopButtonRepeat);

  // Parameters stacks (current window)
  m.def("PushItemWidth",&ImGui::PushItemWidth);
  m.def("PopItemWidth",&ImGui::PopItemWidth);
  m.def("SetNextItemWidth",&ImGui::SetNextItemWidth);
  m.def("CalcItemWidth",&ImGui::CalcItemWidth);
  m.def("PushTextWrapPos",&ImGui::PushTextWrapPos);
  m.def("PopTextWrapPos",&ImGui::PopTextWrapPos);

  // Style read access
  // - Use the style editor (ShowStyleEditor() function) to interactively see what the colors are)
  m.def("GetFont",&ImGui::GetFont);
  m.def("GetFontSize",&ImGui::GetFontSize);
  m.def("GetFontTexUvWhitePixel",&ImGui::GetFontTexUvWhitePixel);
  m.def("GetColorU32",static_cast<ImU32(*)(ImGuiCol,float)>(&ImGui::GetColorU32));
  m.def("GetColorU32",static_cast<ImU32(*)(const ImVec4&)>(&ImGui::GetColorU32));
  m.def("GetColorU32",static_cast<ImU32(*)(ImU32)>(&ImGui::GetColorU32));
  m.def("GetStyleColorVec4",&ImGui::GetStyleColorVec4);

  // Cursor / Layout
  // - By "cursor" we mean the current output position.
  // - The typical widget behavior is to output themselves at the current cursor position, then move the cursor one line down.
  // - You can call SameLine() between widgets to undo the last carriage return and output at the right of the preceding widget.
  // - Attention! We currently have inconsistencies between window-local and absolute positions we will aim to fix with future API:
  //    Window-local coordinates:   SameLine(), GetCursorPos(), SetCursorPos(), GetCursorStartPos(), GetContentRegionMax(), GetWindowContentRegion*(), PushTextWrapPos()
  //    Absolute coordinate:        GetCursorScreenPos(), SetCursorScreenPos(), all ImDrawList:: functions.
  m.def("Separator",&ImGui::Separator);
  m.def("SameLine",&ImGui::SameLine);
  m.def("NewLine",&ImGui::NewLine);
  m.def("Spacing",&ImGui::Spacing);
  m.def("Dummy",&ImGui::Dummy);
  m.def("Indent",&ImGui::Indent);
  m.def("Unindent",&ImGui::Unindent);
  m.def("BeginGroup",&ImGui::BeginGroup);
  m.def("EndGroup",&ImGui::EndGroup);
  m.def("GetCursorPos",&ImGui::GetCursorPos);
  m.def("GetCursorPosX",&ImGui::GetCursorPosX);
  m.def("GetCursorPosY",&ImGui::GetCursorPosY);
  m.def("SetCursorPos",&ImGui::SetCursorPos);
  m.def("SetCursorPosX",&ImGui::SetCursorPosX);
  m.def("SetCursorPosY",&ImGui::SetCursorPosY);
  m.def("GetCursorStartPos",&ImGui::GetCursorStartPos);
  m.def("GetCursorScreenPos",&ImGui::GetCursorScreenPos);
  m.def("SetCursorScreenPos",&ImGui::SetCursorScreenPos);
  m.def("AlignTextToFramePadding",&ImGui::AlignTextToFramePadding);
  m.def("GetTextLineHeight",&ImGui::GetTextLineHeight);
  m.def("GetTextLineHeightWithSpacing",&ImGui::GetTextLineHeightWithSpacing);
  m.def("GetFrameHeight",&ImGui::GetFrameHeight);
  m.def("GetFrameHeightWithSpacing",&ImGui::GetFrameHeightWithSpacing);

  // Widgets: Text
  m.def("Text",[](const char* fmt) {
    ImGui::Text(fmt);
  });
  m.def("TextColored",[](const ImVec4& col,const char* fmt) {
    ImGui::TextColored(col,fmt);
  });
  m.def("TextDisabled",[](const char* fmt) {
    ImGui::TextDisabled(fmt);
  });
  m.def("TextWrapped",[](const char* fmt) {
    ImGui::TextWrapped(fmt);
  });
  m.def("LabelText",[](const char* label,const char* fmt) {
    ImGui::LabelText(label,fmt);
  });
  m.def("BulletText",[](const char* fmt) {
    ImGui::BulletText(fmt);
  });

  // Widgets: Main
  // - Most widgets return true when the value has been changed or when pressed/selected
  // - You may also use one of the many IsItemXXX functions (e.g. IsItemActive, IsItemHovered, etc.) to query widget state.
  m.def("Button",&ImGui::Button);
  m.def("SmallButton",&ImGui::SmallButton);
  m.def("InvisibleButton",&ImGui::InvisibleButton);
  m.def("ArrowButton",&ImGui::ArrowButton);
  m.def("Checkbox",[](const char* label,bool v)->std::tuple<bool,bool> {
    bool ret=ImGui::Checkbox(label,&v);
    return std::make_tuple(ret,v);
  });
  m.def("CheckboxFlags",[](const char* label,int flags,int flags_value)->std::tuple<bool,int> {
    bool ret=ImGui::CheckboxFlags(label,&flags,flags_value);
    return std::make_tuple(ret,flags);
  });
  m.def("RadioButton",[](const char* label,bool active) {
    return ImGui::RadioButton(label,active);
  });
  m.def("RadioButton",[](const char* label,int v,int value) {
    bool ret=ImGui::RadioButton(label,&v,value);
    return std::make_tuple(ret,v);
  });
  m.def("ProgressBar",&ImGui::ProgressBar);
  m.def("Bullet",&ImGui::Bullet);

  // Widgets: Combo Box
  // - The BeginCombo()/EndCombo() api allows you to manage your contents and selection state however you want it, by creating e.g. Selectable() items.
  // - The old Combo() api are helpers over BeginCombo()/EndCombo() which are kept available for convenience purpose. This is analogous to how ListBox are created.
  m.def("BeginCombo",&ImGui::BeginCombo);
  m.def("EndCombo",&ImGui::EndCombo);
  m.def("Combo",[](const char* label,int current_item,const std::vector<std::string>& items) {
    std::vector<const char*> items_c_str;
    for(auto item:items)
      items_c_str.push_back(item.c_str());
    bool ret=ImGui::Combo(label,&current_item,items_c_str.data(),items_c_str.size());
    return std::make_tuple(ret,current_item);
  });

  // Widgets: Drag Sliders
  // - CTRL+Click on any drag box to turn them into an input box. Manually input values aren't clamped and can go off-bounds.
  // - For all the Float2/Float3/Float4/Int2/Int3/Int4 versions of every functions, note that a 'float v[X]' function argument is the same as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible. You can pass address of your first element out of a contiguous set, e.g. &myvector.x
  // - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
  // - Format string may also be set to NULL or use the default format ("%f" or "%d").
  // - Speed are per-pixel of mouse movement (v_speed=0.2f: mouse needs to move by 5 pixels to increase value by 1). For gamepad/keyboard navigation, minimum speed is Max(v_speed, minimum_step_at_given_precision).
  // - Use v_min < v_max to clamp edits to given limits. Note that CTRL+Click manual input can override those limits.
  // - Use v_max = FLT_MAX / INT_MAX etc to avoid clamping to a maximum, same with v_min = -FLT_MAX / INT_MIN to avoid clamping to a minimum.
  // - We use the same sets of flags for DragXXX() and SliderXXX() functions as the features are the same and it makes it easier to swap them.
  // - Legacy: Pre-1.78 there are DragXXX() function signatures that takes a final `float power=1.0f' argument instead of the `ImGuiSliderFlags flags=0' argument.
  //   If you get a warning converting a float to ImGuiSliderFlags, read https://github.com/ocornut/imgui/issues/3361
  m.def("DragFloat",[](const char* label,float v,float v_speed=1.0f,float v_min=0.0f,float v_max=0.0f,const char* format="%.3f",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::DragFloat(label,&v,v_speed,v_min,v_max,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("DragFloat2",[](const char* label,std::array<float,2> v,float v_speed=1.0f,float v_min=0.0f,float v_max=0.0f,const char* format="%.3f",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::DragFloat2(label,&v[0],v_speed,v_min,v_max,format,flags);
    return std::make_tuple(ret,v[0],v[1]);
  });
  m.def("DragFloat3",[](const char* label,std::array<float,3> v,float v_speed=1.0f,float v_min=0.0f,float v_max=0.0f,const char* format="%.3f",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::DragFloat3(label,&v[0],v_speed,v_min,v_max,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("DragFloat4",[](const char* label,std::array<float,4> v,float v_speed=1.0f,float v_min=0.0f,float v_max=0.0f,const char* format="%.3f",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::DragFloat4(label,&v[0],v_speed,v_min,v_max,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("DragFloatRange2",[](const char* label,float v_current_min,float v_current_max,float v_speed=1.0f,float v_min=0.0f,float v_max=0.0f,const char* format="%.3f",const char* format_max=NULL,ImGuiSliderFlags flags=0) {
    bool ret=ImGui::DragFloatRange2(label,&v_current_min,&v_current_max,v_speed,v_min,v_max,format,format_max,flags);
    return std::make_tuple(ret,v_current_min,v_current_max);
  });
  m.def("DragInt",[](const char* label,int v,float v_speed=1.0f,int v_min=0.0f,int v_max=0.0f,const char* format="%d",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::DragInt(label,&v,v_speed,v_min,v_max,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("DragInt2",[](const char* label,std::array<int,2> v,float v_speed=1.0f,int v_min=0.0f,int v_max=0.0f,const char* format="d",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::DragInt2(label,&v[0],v_speed,v_min,v_max,format,flags);
    return std::make_tuple(ret,v[0],v[1]);
  });
  m.def("DragInt3",[](const char* label,std::array<int,3> v,float v_speed=1.0f,int v_min=0.0f,int v_max=0.0f,const char* format="%d",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::DragInt3(label,&v[0],v_speed,v_min,v_max,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("DragInt4",[](const char* label,std::array<int,4> v,float v_speed=1.0f,int v_min=0.0f,int v_max=0.0f,const char* format="%d",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::DragInt4(label,&v[0],v_speed,v_min,v_max,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("DragIntRange2",[](const char* label,int v_current_min,int v_current_max,float v_speed=1.0f,int v_min=0.0f,int v_max=0.0f,const char* format="%d",const char* format_max=NULL,ImGuiSliderFlags flags=0) {
    bool ret=ImGui::DragIntRange2(label,&v_current_min,&v_current_max,v_speed,v_min,v_max,format,format_max,flags);
    return std::make_tuple(ret,v_current_min,v_current_max);
  });

  // Widgets: Regular Sliders
  // - CTRL+Click on any slider to turn them into an input box. Manually input values aren't clamped and can go off-bounds.
  // - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
  // - Format string may also be set to NULL or use the default format ("%f" or "%d").
  // - Legacy: Pre-1.78 there are SliderXXX() function signatures that takes a final `float power=1.0f' argument instead of the `ImGuiSliderFlags flags=0' argument.
  //   If you get a warning converting a float to ImGuiSliderFlags, read https://github.com/ocornut/imgui/issues/3361
  m.def("SliderFloat",[](const char* label,float v,float v_min=0.0f,float v_max=0.0f,const char* format="%.3f",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::SliderFloat(label,&v,v_min,v_max,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("SliderFloat2",[](const char* label,std::array<float,2> v,float v_min=0.0f,float v_max=0.0f,const char* format="%.3f",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::SliderFloat2(label,&v[0],v_min,v_max,format,flags);
    return std::make_tuple(ret,v[0],v[1]);
  });
  m.def("SliderFloat3",[](const char* label,std::array<float,3> v,float v_min=0.0f,float v_max=0.0f,const char* format="%.3f",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::SliderFloat3(label,&v[0],v_min,v_max,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("SliderFloat4",[](const char* label,std::array<float,4> v,float v_min=0.0f,float v_max=0.0f,const char* format="%.3f",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::SliderFloat4(label,&v[0],v_min,v_max,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("SliderAngle",[](const char* label,float v,float v_degrees_min=-360.0f,float v_degrees_max=+360.0f,const char* format="%.0f deg",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::SliderAngle(label,&v,v_degrees_min,v_degrees_max,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("SliderInt",[](const char* label,int v,int v_min=0.0f,int v_max=0.0f,const char* format="%d",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::SliderInt(label,&v,v_min,v_max,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("SliderInt2",[](const char* label,std::array<int,2> v,int v_min=0.0f,int v_max=0.0f,const char* format="d",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::SliderInt2(label,&v[0],v_min,v_max,format,flags);
    return std::make_tuple(ret,v[0],v[1]);
  });
  m.def("SliderInt3",[](const char* label,std::array<int,3> v,int v_min=0.0f,int v_max=0.0f,const char* format="%d",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::SliderInt3(label,&v[0],v_min,v_max,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("SliderInt4",[](const char* label,std::array<int,4> v,int v_min=0.0f,int v_max=0.0f,const char* format="%d",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::SliderInt4(label,&v[0],v_min,v_max,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("VSliderFloat",[](const char* label,const ImVec2& size,float v,float v_min=0.0f,float v_max=0.0f,const char* format="%.3f",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::VSliderFloat(label,size,&v,v_min,v_max,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("VSliderInt",[](const char* label,const ImVec2& size,int v,int v_min=0.0f,int v_max=0.0f,const char* format="%d",ImGuiSliderFlags flags=0) {
    bool ret=ImGui::VSliderInt(label,size,&v,v_min,v_max,format,flags);
    return std::make_tuple(ret,v);
  });

  // Widgets: Input with Keyboard
  // - If you want to use InputText() with std::string or any custom dynamic string type, see misc/cpp/imgui_stdlib.h and comments in imgui_demo.cpp.
  // - Most of the ImGuiInputTextFlags flags are only useful for InputText() and not for InputFloatX, InputIntX, InputDouble etc.
  m.def("InputText",[](const char* label,const std::string& bufStr,ImGuiInputTextFlags flags=0) {
    size_t buf_size=1024;
    char buf[buf_size];
    strcpy(buf,bufStr.c_str());
    bool ret=ImGui::InputText(label,buf,buf_size,flags);
    return std::make_tuple(ret,std::string(buf));
  });
  m.def("InputTextMultiline",[](const char* label,const std::string& bufStr,const ImVec2& size=ImVec2(0,0),ImGuiInputTextFlags flags=0) {
    size_t buf_size=1024;
    char buf[buf_size];
    strcpy(buf,bufStr.c_str());
    bool ret=ImGui::InputTextMultiline(label,buf,buf_size,size,flags);
    return std::make_tuple(ret,std::string(buf));
  });
  m.def("InputTextWithHint",[](const char* label,const char* hint,const std::string& bufStr,ImGuiInputTextFlags flags=0) {
    size_t buf_size=1024;
    char buf[buf_size];
    strcpy(buf,bufStr.c_str());
    bool ret=ImGui::InputTextWithHint(label,hint,buf,buf_size,flags);
    return std::make_tuple(ret,std::string(buf));
  });
  m.def("InputFloat",[](const char* label,float v,float step=0.0f,float step_fast=0.0f,const char* format="%.3f",ImGuiInputTextFlags flags=0) {
    bool ret=ImGui::InputFloat(label,&v,step,step_fast,format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("InputFloat2",[](const char* label,std::array<float,2> v,float step=0.0f,float step_fast=0.0f,const char* format="%.3f",ImGuiInputTextFlags flags=0) {
    bool ret=ImGui::InputFloat2(label,&v[0],format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("InputFloat3",[](const char* label,std::array<float,3> v,float step=0.0f,float step_fast=0.0f,const char* format="%.3f",ImGuiInputTextFlags flags=0) {
    bool ret=ImGui::InputFloat3(label,&v[0],format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("InputFloat4",[](const char* label,std::array<float,4> v,float step=0.0f,float step_fast=0.0f,const char* format="%.3f",ImGuiInputTextFlags flags=0) {
    bool ret=ImGui::InputFloat4(label,&v[0],format,flags);
    return std::make_tuple(ret,v);
  });
  m.def("InputInt",[](const char* label,int v,int step=1,int step_fast=100,ImGuiInputTextFlags flags=0) {
    bool ret=ImGui::InputInt(label,&v,step,step_fast,flags);
    return std::make_tuple(ret,v);
  });
  m.def("InputInt2",[](const char* label,std::array<int,2> v,int step=1,int step_fast=100,ImGuiInputTextFlags flags=0) {
    bool ret=ImGui::InputInt2(label,&v[0],flags);
    return std::make_tuple(ret,v);
  });
  m.def("InputInt3",[](const char* label,std::array<int,3> v,int step=1,int step_fast=100,ImGuiInputTextFlags flags=0) {
    bool ret=ImGui::InputInt3(label,&v[0],flags);
    return std::make_tuple(ret,v);
  });
  m.def("InputInt4",[](const char* label,std::array<int,4> v,int step=1,int step_fast=100,ImGuiInputTextFlags flags=0) {
    bool ret=ImGui::InputInt4(label,&v[0],flags);
    return std::make_tuple(ret,v);
  });
}

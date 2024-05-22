#pragma once

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif



#include "imgui\imgui.h"
#include "imgui\imgui_internal.h"




// System includes
#include <ctype.h>      // toupper
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>     // intptr_t
#else
#include <stdint.h>     // intptr_t
#endif

using namespace std;
#include <map>


class ImGuiUI {
public:
	bool    Checkbox(const char* label, bool* v);

	bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.2f", ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	bool          BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags = 0);

	void EndCombo();

	bool          Selectable(const char* label, bool selected = false, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0)); // "bool selected" carry the selection state (read-only). Selectable() is clicked is returns true so you can modify your selection state. size.x==0.0: use remaining width, size.x>0.0: specify width. size.y==0.0: use label height, size.y>0.0: specify height

	bool beginChild(const char* name, ImVec2 pos, ImVec2 size);

	void endChild();

	bool imageSliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags);

	bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	bool SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags);
	
	bool Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
	bool          Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1);
	bool begin_child(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0);

	void end_child();

	bool begin_child_ex(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags);


	ImVec2 CalcTextSize_ex(ImFont* font_Cale, const char* text, const char* text_end = NULL, bool hide_text_after_double_hash = false, float wrap_width = -1.0f);
	bool Text(const char* label,  const char* extid = 0, float wra = 0.0f,int sty=0);

	bool imageSelectable(ImTextureID image, ImFont* font, const char* icon, float fontsize, ImVec2 framesize, void* p_data, const void* p_min, const void* p_max, const char* format ="%.2f", bool selected = false);

	bool iconSelectable(ImFont* font, const char* icon, float fontsize, ImVec2 framesize, bool selected = false);

	bool iconSelectable(ImFont* font, const char* icon, const char* label, float fontsize, ImVec2 framesize = ImVec2(47, 47), bool selected = false);



	bool          Button(const char* label, const char* button_label, const ImVec2& size_arg = ImVec2(0, 0), ImGuiButtonFlags flags = ImGuiButtonFlags_None);

	bool Button(int style, ImFont* font, float font_size, const char* icon, const char* label, ImColor color_text = ImColor(235, 235, 235), ImColor color_held = ImColor(80, 200, 160), ImVec2 framesize = { 47,47 }, bool selected = false);



	

	bool Button0(ImFont* font, float font_size, const char* icon, const char* label, ImColor color_text = ImColor(235, 235, 235), ImColor color_held = ImColor(80, 200, 160), ImVec2 framesize = { 47,47 }, bool selected = false);

	bool Button(ImFont* font, float font_size, const char* icon, const char* label, ImColor color_text = ImColor(235, 235, 235), ImColor color_held = ImColor(80, 200, 160), ImVec2 framesize = { 47,47 }, bool selected = false);



	void setNextSubWidget();

	void doFade(ImVec2& pos, const char* id, ImVec2 winSize, bool canDoFade);

	float anim_move_t = 0.1f;


	ImDrawList* draw_list = NULL;
	std::map <string, ImGuiUI*> ui_map;
	std::map <string, ImVec2> anim_move_iconSelectable;
};
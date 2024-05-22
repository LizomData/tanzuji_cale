#include "imGuiUI.h"

using namespace ImGui;


static inline ImVec2  operator*(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2  operator/(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2  operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2  operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2  operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2  operator/(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline ImVec2  operator-(const ImVec2& lhs) { return ImVec2(-lhs.x, -lhs.y); }
static inline bool  operator<(const ImVec2& lhs, const ImVec2& rhs) { return (lhs.x < rhs.x) && (lhs.y < rhs.y); }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
static inline ImVec4  operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline ImVec4  operator-(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline ImVec4  operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
static inline ImVec4  operator/(const ImVec4& lhs, const float rhs) { return ImVec4(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs); }





struct checkbox_animation {
	float animation;
};

bool ImGuiUI::Checkbox(const char* label, bool* v)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	ImVec2 pos = window->DC.CursorPos;
	const ImVec2 winSize = window->Size;

	{
		//static std::map <ImGuiID, ImVec2> anim_move;
		//auto it_anim = anim_move.find(id);
		//if (it_anim == anim_move.end())
		//{
		//	anim_move.insert({ id, pos });
		//	it_anim = anim_move.find(id);
		//}

		//it_anim->second = ImLerp(it_anim->second, pos, ImGui::GetIO().DeltaTime / anim_move_t);
		//if (g.MovingWindow != NULL)
		//	it_anim->second = pos;
		//pos = it_anim->second;
		doFade(pos, label, winSize, g.MovingWindow == NULL);

	}


	//控件框架位置
	const ImRect frame = { pos , pos + ImVec2(winSize.x,  45.0f) };

	//标签位置
	ImRect label_bb(frame.Min + ImVec2(30.0f, frame.GetHeight() / 2.0f - label_size.y / 2.0f), { 0,0 });
	label_bb.Max = label_bb.Min + label_size;
	//状态球背景大小
	const ImVec2 squareSize = { 17 + 13,17 };

	//状态球位置	
	ImRect frame_bb(frame.Min + ImVec2(frame.GetWidth() - 30 - squareSize.x, frame.GetHeight() / 2.0f - squareSize.y / 2.0f), { 0.0f , 0.0f });
	frame_bb.Max = (frame_bb.Min + squareSize);
	ItemSize(frame, style.FramePadding.y);
	if (!ItemAdd(frame, id))
	{
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
		return false;
	}


	bool hovered, held, pressed;
	{
		ImRect frame_bb_tem;
		frame_bb_tem.Min = frame_bb.Min - ImVec2(frame_bb.GetWidth(), frame_bb.GetHeight()) * 0.5f;
		frame_bb_tem.Max = frame_bb.Max + ImVec2(frame_bb.GetWidth(), frame_bb.GetHeight()) * 0.5f;
		pressed = ButtonBehavior(frame_bb_tem, id, &hovered, &held);
		if (pressed)
		{
			*v = !(*v);
			MarkItemEdited(id);
		}
	}





	static std::map <ImGuiID, checkbox_animation> anim;
	auto it_anim = anim.find(id);
	if (it_anim == anim.end())
	{
		anim.insert({ id, { 0.0f } });
		it_anim = anim.find(id);
	}

	it_anim->second.animation = ImLerp(it_anim->second.animation, *v ? 1.0f : 0.0f, ImGui::GetIO().DeltaTime / 0.12f);

	RenderNavHighlight(label_bb, id);

	//控件背景
	{
		bool hovered = ItemHoverable(frame, id, 0);

		static std::map <ImGuiID, ImVec4> anim;
		auto it_anim = anim.find(id);
		if (it_anim == anim.end())
		{
			anim.insert({ id, ImVec4(200, 200, 200, 0) });
			it_anim = anim.find(id);
		}

		it_anim->second = ImLerp(it_anim->second, hovered ? ImVec4(200, 200, 200, 50) : ImVec4(200, 200, 200, 0), ImGui::GetIO().DeltaTime / 0.25f);
		const ImU32 frame_bg = ImColor(it_anim->second / 255.0f);
		draw_list->AddRectFilled(frame.Min, frame.Max, frame_bg,5.0f);
	}

	//状态球背景
	RenderFrame(frame_bb.Min, frame_bb.Max, ImColor(255, 255, 255, 100), false, 9.0f);
	RenderFrame(frame_bb.Min, frame_bb.Max, ImColor(147 / 255.0f, 190 / 255.0f, 66 / 255.0f, it_anim->second.animation), false, 9.0f);



	//状态球
	const ImU32 circle_col = *v ? ImColor(1.0f, 1.0f, 1.0f) : ImColor(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f);
	GetWindowDrawList()->AddCircleFilled(ImVec2(frame_bb.Min.x + 8 + 14 * it_anim->second.animation, frame_bb.Min.y + 8), 5.0f, circle_col, 30);



	if (label_size.x > 0.0f)
		GetWindowDrawList()->AddText(label_bb.Min, ImColor(255, 255, 255), label);
		

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
	return pressed;
}

bool ImGuiUI::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
}


bool ImGuiUI::SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}


struct slider_element {
	float opacity;
};

// FIXME-LEGACY: Prior to 1.61 our DragInt() function internally used floats and because of this the compile-time default value for format was "%.0f".
// Even though we changed the compile-time default, we expect users to have carried %f around, which would break the display of DragInt() calls.
// To honor backward compatibility we are rewriting the format string, unless IMGUI_DISABLE_OBSOLETE_FUNCTIONS is enabled. What could possibly go wrong?!
static const char* PatchFormatStringFloatToInt(const char* fmt)
{
	if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
		return "%d";
	const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
	const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
	if (fmt_end > fmt_start && fmt_end[-1] == 'f')
	{
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
		if (fmt_start == fmt && fmt_end[0] == 0)
			return "%d";
		const char* tmp_format;
		ImFormatStringToTempBuffer(&tmp_format, NULL, "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
		return tmp_format;
#else
		IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
	}
	return fmt;
}

bool ImGuiUI::SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const float w = GetWindowWidth() - 30;
	ImVec2 pos = window->DC.CursorPos;
	const ImVec2 winSize = window->Size;
	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const ImVec2 grab_size = ImVec2(180.0f, 8.0f);


	{
		//static std::map <ImGuiID, ImVec2> anim_move;
		//auto it_anim = anim_move.find(id);
		//if (it_anim == anim_move.end())
		//{
		//	anim_move.insert({ id, pos });
		//	it_anim = anim_move.find(id);
		//}

		//it_anim->second = ImLerp(it_anim->second, pos, ImGui::GetIO().DeltaTime / anim_move_t);
		//if (g.MovingWindow != NULL)
		//	it_anim->second = pos;
		//pos = it_anim->second;
		doFade(pos, label, winSize, g.MovingWindow == NULL);

	}
	//控件框架位置
	const ImRect frame = { pos , pos + ImVec2(winSize.x,  45.0f) };

	//滑块位置
	ImRect frame_bb(frame.Min + ImVec2(frame.GetWidth() - 30 - grab_size.x, frame.GetHeight() / 2.0f - grab_size.y / 2.0f), { 0,0 });
	frame_bb.Max = frame_bb.Min + grab_size;

	//标签位置
	ImRect label_bb(frame.Min + ImVec2(30.0f, frame.GetHeight() / 2.0f - label_size.y / 2.0f), { 0,0 });
	label_bb.Max = ImVec2(frame_bb.Min.x - 30, label_bb.Min.y);

	ItemSize(frame, style.FramePadding.y);
	if (!ItemAdd(frame, id, &frame_bb))
		return false;

	// Default format string when passing NULL
	if (format == NULL)
		format = DataTypeGetInfo(data_type)->PrintFmt;
	else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
		format = PatchFormatStringFloatToInt(format);

	// Tabbing or CTRL-clicking on Slider turns it into an input box
	const bool hovered = ItemHoverable(frame_bb, id, 0);
	const bool clicked = (hovered && g.IO.MouseClicked[0]);
	if (clicked)
	{
		SetActiveID(id, window);
		SetFocusID(id, window);
		FocusWindow(window);
		g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
	}

	static std::map <ImGuiID, slider_element> anim;
	auto it_anim = anim.find(id);
	if (it_anim == anim.end())
	{
		anim.insert({ id, { 0.0f } });
		it_anim = anim.find(id);
	}

	it_anim->second.opacity = ImLerp(it_anim->second.opacity, IsItemActive() ? 1.0f : 0.4f, 0.08f * (1.0f - ImGui::GetIO().DeltaTime));

	ImRect grab_bb;
	bool value_changed;
	{
		ImRect frame_bb_tem = frame_bb;

		frame_bb_tem.Max = frame_bb_tem.Max + ImVec2(12, 0);

		value_changed = SliderBehavior(frame_bb_tem, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
		if (value_changed)
			MarkItemEdited(id);
	}


	char value_buf[64];
	const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

	//控件背景
	{
		bool hovered = ItemHoverable(frame, id, 0);

		static std::map <ImGuiID, ImVec4> anim;
		auto it_anim = anim.find(id);
		if (it_anim == anim.end())
		{
			anim.insert({ id, ImVec4(200, 200, 200, 0) });
			it_anim = anim.find(id);
		}

		it_anim->second = ImLerp(it_anim->second, hovered ? ImVec4(200, 200, 200, 50) : ImVec4(200, 200, 200, 0), ImGui::GetIO().DeltaTime / 0.25f);
		const ImU32 frame_bg = ImColor(it_anim->second / 255.0f);
		draw_list->AddRectFilled(frame.Min, frame.Max, frame_bg);
	}


	draw_list->AddRectFilled(frame_bb.Min, frame_bb.Max, ImColor(15, 15, 16,180), 5.0f);
	draw_list->AddRectFilled(frame_bb.Min, ImVec2(grab_bb.Min.x + 3, frame_bb.Max.y), ImColor(147, 190, 66), 5.0f);
	draw_list->AddCircleFilled(ImVec2(grab_bb.Min.x - 1, grab_bb.Min.y + 2), 6.0f, ImColor(1.0f, 1.0f, 1.0f), 30);

	window->DrawList->AddText(label_bb.Min, ImColor(255, 255, 255), label);


	PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, it_anim->second.opacity)); {
		RenderTextClipped(label_bb.Min, { frame_bb.Min.x - 30.0f,frame.Max.y }, value_buf, value_buf_end, NULL, ImVec2(1.f, 0.f));
	} PopStyleColor();

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
	return value_changed;
}


// Getter for the old Combo() API: const char*[]
static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char* const* items = (const char* const*)data;
	if (out_text)
		*out_text = items[idx];
	return true;
}
static float CalcMaxPopupHeightFromItemCount(int items_count)
{
	ImGuiContext& g = *GImGui;
	if (items_count <= 0)
		return FLT_MAX;
	return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}
// Old API, prefer using BeginCombo() nowadays if you can.
bool ImGuiUI::Combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
{
	ImGuiContext& g = *GImGui;

	// Call the getter to obtain the preview string which is a parameter to BeginCombo()
	const char* preview_value = NULL;
	if (*current_item >= 0 && *current_item < items_count)
		items_getter(data, *current_item, &preview_value);

	// The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
	if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
		SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

	if (!BeginCombo(label, preview_value, ImGuiComboFlags_None))
		return false;

	// Display items
	// FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
	bool value_changed = false;
	for (int i = 0; i < items_count; i++)
	{
		PushID(i);
		const bool item_selected = (i == *current_item);
		const char* item_text;
		if (!items_getter(data, i, &item_text))
			item_text = "*Unknown item*";
		if (Selectable(item_text, item_selected))
		{
			value_changed = true;
			*current_item = i;
		}
		if (item_selected)
			SetItemDefaultFocus();
		PopID();
	}

	EndCombo();

	if (value_changed)
		MarkItemEdited(g.LastItemData.ID);

	return value_changed;
}

// Combo box helper allowing to pass an array of strings.
bool ImGuiUI::Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
{
	const bool value_changed = Combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
	return value_changed;
}
struct combo_element {
	float open_anim, arrow_anim;
};
bool ImGuiUI::BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();
	ImVec2 pos = window->DC.CursorPos;
	const ImVec2 winSize = window->Size;
	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const ImVec2 buttonlabel_size = CalcTextSize(preview_value, NULL, true);
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	{
		//static std::map <ImGuiID, ImVec2> anim_move;
		//auto it_anim = anim_move.find(id);
		//if (it_anim == anim_move.end())
		//{
		//	anim_move.insert({ id, pos });
		//	it_anim = anim_move.find(id);
		//}

		//it_anim->second = ImLerp(it_anim->second, pos, ImGui::GetIO().DeltaTime / anim_move_t);
		//if (g.MovingWindow != NULL)
		//	it_anim->second = pos;
		//pos = it_anim->second;
		doFade(pos, label, winSize, g.MovingWindow == NULL);

	}

	//控件框架位置
	const ImRect frame = { pos , pos + ImVec2(winSize.x,  45.0f) };

	ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
	g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
	if (window->SkipItems)
		return false;


	IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together
	const float size = GetWindowWidth() - 30;
	ImVec2 squareSize = { 180,30 };
	//标签位置
	ImRect label_bb(frame.Min + ImVec2(30.0f, frame.GetHeight() / 2.0f - label_size.y / 2.0f), { 0,0 });
	label_bb.Max = label_bb.Min + label_size;

	//按钮背景
	ImRect frame_bb(frame.Min + ImVec2(frame.GetWidth() - 30 - squareSize.x, frame.GetHeight() / 2.0f - squareSize.y / 2.0f), { 0.0f , 0.0f });
	frame_bb.Max = (frame_bb.Min + squareSize);

	// 按钮标签位置(上下左右居中)
	ImRect buttonlabel_bb(frame_bb.Min + ImVec2(10, frame_bb.GetHeight() / 2.0f - buttonlabel_size.y / 2.0f), { 0,0 });
	buttonlabel_bb.Max = buttonlabel_bb.Min + buttonlabel_size;

	ImRect clickable(window->DC.CursorPos + ImVec2(0, 23), window->DC.CursorPos + ImVec2(size, 53));
	clickable = frame_bb;
	const ImVec2 arrow_size = ImVec2(9, 9);

	ItemSize(frame, style.FramePadding.y);
	if (!ItemAdd(frame, id, &frame))
		return false;

	//控件背景
	{
		bool hovered = ItemHoverable(frame, id, 0);

		static std::map <ImGuiID, ImVec4> anim;
		auto it_anim = anim.find(id);
		if (it_anim == anim.end())
		{
			anim.insert({ id, ImVec4(255, 255, 255, 25) });
			it_anim = anim.find(id);
		}

		it_anim->second = ImLerp(it_anim->second, hovered ? ImVec4(200, 200, 200, 50) : ImVec4(255, 255, 255, 25), ImGui::GetIO().DeltaTime / 0.25f);
		const ImU32 frame_bg = ImColor(it_anim->second / 255.0f);
		draw_list->AddRectFilled(frame.Min, frame.Max, frame_bg);
	}

	// Open on click
	bool hovered, held;
	bool pressed = ButtonBehavior(clickable, id, &hovered, &held);
	const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id);
	bool popup_open = IsPopupOpen(popup_id, ImGuiPopupFlags_None);
	if (pressed && !popup_open)
	{
		OpenPopupEx(popup_id, ImGuiPopupFlags_None);
		popup_open = true;
	}

	static std::map <ImGuiID, combo_element> anim;
	auto it_anim = anim.find(id);
	if (it_anim == anim.end())
	{
		anim.insert({ id, { 0.0f, 0.0f } });
		it_anim = anim.find(id);
	}

	it_anim->second.open_anim = ImLerp(it_anim->second.open_anim, popup_open ? 1.0f : 0.0f, 0.04f * (1.0f - ImGui::GetIO().DeltaTime));
	it_anim->second.arrow_anim = ImLerp(it_anim->second.arrow_anim, popup_open ? 0.3f : 0.0f, 0.05f * (1.0f - ImGui::GetIO().DeltaTime));

	draw_list->AddRectFilled(clickable.Min, clickable.Max, ImColor(80, 200, 160, 50), 3.0f);
	draw_list->AddRect(clickable.Min, clickable.Max, ImColor(1.0f, 1.0f, 1.0f, 0.1f), 3.0f);

	draw_list->AddText(label_bb.Min, GetColorU32(ImGuiCol_Text), label);

	if (!(flags & ImGuiComboFlags_NoArrowButton))
	{

		RenderArrow(draw_list, frame_bb.Max - ImVec2(30, 23), ImColor(80, 200, 160, 100), ImGuiDir_Down, 1.0f);
	}

	RenderTextClipped(buttonlabel_bb.Min, buttonlabel_bb.Max, preview_value, NULL, NULL, ImVec2(0.0f, 0.0f));

	/*PushFont(combo_arrow); {
		draw_list->AddText(ImVec2(clickable.Max.x - 20, (clickable.Min.y + clickable.Max.y) / 2 - arrow_size.y / 2), ImColor(1.0f, 1.0f, 1.0f, 0.2f + it_anim->second.arrow_anim), "A");
	} PopFont();*/

	if (!popup_open)
		return false;

	g.NextWindowData.Flags = backup_next_window_data_flags;
	if (!IsPopupOpen(popup_id, ImGuiPopupFlags_None))
	{
		g.NextWindowData.ClearFlags();
		return false;
	}

	// Set popup size
	float w = clickable.GetWidth();
	if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint)
	{
		g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
	}
	else
	{
		if ((flags & ImGuiComboFlags_HeightMask_) == 0)
			flags |= ImGuiComboFlags_HeightRegular;
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_)); // Only one
		int popup_max_height_in_items = -1;
		if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
		else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
		else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
		SetNextWindowSizeConstraints(ImVec2(w, 0.0f), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items) * it_anim->second.open_anim));
	}

	// This is essentially a specialized version of BeginPopupEx()
	char name[16];
	ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

	// Set position given a custom constraint (peak into expected window size so we can position it)
	// FIXME: This might be easier to express with an hypothetical SetNextWindowPosConstraints() function?
	// FIXME: This might be moved to Begin() or at least around the same spot where Tooltips and other Popups are calling FindBestWindowPosForPopupEx()?
	if (ImGuiWindow* popup_window = FindWindowByName(name))
		if (popup_window->WasActive)
		{
			// Always override 'AutoPosLastDirection' to not leave a chance for a past value to affect us.
			ImVec2 size_expected = CalcWindowNextAutoFitSize(popup_window);
			popup_window->AutoPosLastDirection = (flags & ImGuiComboFlags_PopupAlignLeft) ? ImGuiDir_Left : ImGuiDir_Down; // Left = "Below, Toward Left", Down = "Below, Toward Right (default)"
			ImRect r_outer = GetPopupAllowedExtentRect(popup_window);
			ImVec2 pos = FindBestWindowPosForPopupEx(clickable.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, clickable, ImGuiPopupPositionPolicy_ComboBox);
			SetNextWindowPos(pos + ImVec2(0, 3));
		}

	// We don't use BeginPopupEx() solely because we have a custom name string, which we could make an argument to BeginPopupEx()
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;
	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 5)); // Horizontally align ourselves with the framed text
	PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);
	PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0f);
	PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.03f));
	PushStyleColor(ImGuiCol_PopupBg, ImVec4(ImColor(34, 34, 36)));
	bool ret = Begin(name, NULL, window_flags | ImGuiWindowFlags_NoScrollbar);
	PopStyleVar(3);
	PopStyleColor(2);
	if (!ret)
	{
		EndPopup();
		IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
		return false;
	}
	return true;
}
void ImGuiUI::EndCombo()
{
	EndPopup();
}
bool ImGuiUI::begin_child_ex(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* parent_window = g.CurrentWindow;

	flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;
	flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);  // Inherit the NoMove flag

	// Size
	const ImVec2 content_avail = GetContentRegionAvail();
	ImVec2 size = ImFloor(size_arg);
	const int auto_fit_axises = ((size.x == 0.0f) ? (1 << ImGuiAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << ImGuiAxis_Y) : 0x00);
	if (size.x <= 0.0f)
		size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)
	if (size.y <= 0.0f)
		size.y = ImMax(content_avail.y + size.y, 4.0f);

	SetNextWindowPos(ImVec2(parent_window->DC.CursorPos.x, parent_window->DC.CursorPos.y + 34.0f));
	SetNextWindowSize(size - ImVec2(0, 36));

	parent_window->DrawList->AddRectFilled(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, size.y), ImColor(24, 24, 26), 4.0f);
	parent_window->DrawList->AddRect(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, size.y), ImColor(1.0f, 1.0f, 1.0f, 0.03f), 4.0f);
	parent_window->DrawList->AddLine(parent_window->DC.CursorPos + ImVec2(1, 32), parent_window->DC.CursorPos + ImVec2(size.x - 1, 32), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
	//parent_window->DrawList->AddRectFilledMultiColorRounded(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x - 115, 32), ImColor(24, 24, 26), ImColor(147, 190, 66, 50), ImColor(147, 190, 66, 0), ImColor(147, 190, 66, 0), ImColor(147, 190, 66, 50), 4.0f, ImDrawCornerFlags_TopLeft);
	//parent_window->DrawList->AddText(bold, 15.0f, parent_window->DC.CursorPos + ImVec2(16, 9), ImColor(147, 190, 66), name);

	// Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(ImGuiID id) with a stable value.
	const char* temp_window_name;
	if (name)
		ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
	else
		ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

	const float backup_border_size = g.Style.ChildBorderSize;
	if (!border)
		g.Style.ChildBorderSize = 0.0f;
	bool ret = Begin(temp_window_name, NULL, flags);
	g.Style.ChildBorderSize = backup_border_size;

	ImGuiWindow* child_window = g.CurrentWindow;
	child_window->ChildId = id;
	//child_window->AutoFitChildAxises = (ImS8)auto_fit_axises;

	// Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
	// While this is not really documented/defined, it seems that the expected thing to do.
	if (child_window->BeginCount == 1)
		parent_window->DC.CursorPos = child_window->Pos;

	// Process navigation-in immediately so NavInit can run on first frame
	//if (g.NavActivateId == id && !(flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavHasScroll))
	//{
	//	FocusWindow(child_window);
	//	NavInitWindow(child_window, false);
	//	SetActiveID(id + 1, child_window); // Steal ActiveId with another arbitrary id so that key-press won't activate child item
	//	g.ActiveIdSource = ImGuiInputSource_Nav;
	//}
	return ret;
}


bool ImGuiUI::begin_child(const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 11));
	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
	return begin_child_ex(str_id, window->GetID(str_id), size_arg, border, extra_flags | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove);
}
void ImGuiUI::end_child()
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;

	IM_ASSERT(g.WithinEndChild == false);
	IM_ASSERT(window->Flags & ImGuiWindowFlags_ChildWindow);   // Mismatched BeginChild()/EndChild() calls

	g.WithinEndChild = true;
	//if (window->BeginCount > 1)
	//{
	//	End();
	//}
	//else
	//{
	//	ImVec2 sz = window->Size;
	//	if (window->AutoFitChildAxises & (1 << ImGuiAxis_X)) // Arbitrary minimum zero-ish child size of 4.0f causes less trouble than a 0.0f
	//		sz.x = ImMax(4.0f, sz.x);
	//	if (window->AutoFitChildAxises & (1 << ImGuiAxis_Y))
	//		sz.y = ImMax(4.0f, sz.y);
	//	End();

	//	ImGuiWindow* parent_window = g.CurrentWindow;
	//	ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
	//	ItemSize(sz);
	//	if ((window->DC.NavLayersActiveMask != 0 || window->DC.NavHasScroll) && !(window->Flags & ImGuiWindowFlags_NavFlattened))
	//	{
	//		ItemAdd(bb, window->ChildId);
	//		RenderNavHighlight(bb, window->ChildId);

	//		// When browsing a window that has no activable items (scroll only) we keep a highlight on the child (pass g.NavId to trick into always displaying)
	//		if (window->DC.NavLayersActiveMask == 0 && window == g.NavWindow)
	//			RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_TypeThin);
	//	}
	//	else
	//	{
	//		// Not navigable into
	//		ItemAdd(bb, 0);
	//	}
	//	if (g.HoveredWindow == window)
	//		g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
	//}

	ImGui::PopStyleVar(2);
	g.WithinEndChild = false;
	g.LogLinePosY = -FLT_MAX; // To enforce a carriage return
}
bool ImGuiUI::Button(const char* label, const char* button_label, const ImVec2& size_arg, ImGuiButtonFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const ImVec2 buttonlabel_size = CalcTextSize(button_label, NULL, true);
	ImVec2 pos = window->DC.CursorPos;
	const ImVec2 winSize = window->Size;

	{
		//static std::map <ImGuiID, ImVec2> anim_move;
		//auto it_anim = anim_move.find(id);
		//if (it_anim == anim_move.end())
		//{
		//	anim_move.insert({ id, pos });
		//	it_anim = anim_move.find(id);
		//}

		//it_anim->second = ImLerp(it_anim->second, pos, ImGui::GetIO().DeltaTime / anim_move_t);
		//if (g.MovingWindow != NULL)
		//	it_anim->second = pos;
		//pos = it_anim->second;

		doFade(pos, label, winSize, g.MovingWindow == NULL);

	}


	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 buttonlabel_Padding = { 60.0f ,6.0f };//按钮标签的空隙
	ImVec2 button_size = CalcItemSize(size_arg, buttonlabel_size.x + buttonlabel_Padding.x, buttonlabel_size.y + buttonlabel_Padding.y);

	//控件框架位置
	const ImRect frame = { pos , pos + button_size };

	//按钮背景位置
	ImRect frame_bb(frame.Min + ImVec2(frame.GetWidth() / 2.0f - button_size.x / 2.0f, frame.GetHeight() / 2.0f - button_size.y / 2.0f), { 0,0 });
	frame_bb.Max = frame_bb.Min + button_size;

	//按钮标签位置(上下左右居中)
	ImRect buttonlabel_bb(frame_bb.Min + ImVec2(frame_bb.GetWidth() / 2.0f - buttonlabel_size.x / 2.0f, frame_bb.GetHeight() / 2.0f - buttonlabel_size.y / 2.0f), { 0,0 });
	buttonlabel_bb.Max = buttonlabel_bb.Min + buttonlabel_size;

	ItemSize(frame, style.FramePadding.y);
	if (!ItemAdd(frame, id))
		return false;

	if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
		flags |= ImGuiButtonFlags_Repeat;

	bool hovered, held;
	bool pressed = ButtonBehavior(frame_bb, id, &hovered, &held, flags);


	// Render
	//控件背景
	//{


	//	bool hovered = ItemHoverable(frame, id, 0);

	//	static std::map <ImGuiID, ImVec4> anim;
	//	auto it_anim = anim.find(id);
	//	if (it_anim == anim.end())
	//	{
	//		anim.insert({ id, ImVec4(255, 255, 255, 25) });
	//		it_anim = anim.find(id);
	//	}

	//	it_anim->second = ImLerp(it_anim->second, hovered ? ImVec4(55, 61, 57, 50) : ImVec4(55, 61, 57, 25), ImGui::GetIO().DeltaTime / 0.25f);
	//	const ImU32 frame_bg = ImColor(it_anim->second / 255.0f);
	//	draw_list->AddRectFilled(frame.Min, frame.Max, frame_bg);
	//}

	const ImU32 col = hovered ? (held ? ImColor(48, 54, 50, 255) : ImColor(48, 54, 50, 200)) : ImColor(48, 54, 50, 150);
	RenderNavHighlight(frame_bb, id);
	RenderFrame(frame_bb.Min, frame_bb.Max, col, true, style.FrameRounding);




	if (g.LogEnabled)
		LogSetNextTextDecoration("[", "]");
	RenderTextClipped(buttonlabel_bb.Min, buttonlabel_bb.Max, button_label, NULL, &label_size, style.ButtonTextAlign, &frame_bb);

	// Automatically close popups
	//if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
	//    CloseCurrentPopup();

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	return pressed;
}
bool ImGuiUI::Button(int style, ImFont* font, float font_size, const char* icon, const char* label, ImColor color_text, ImColor color_held, ImVec2 framesize, bool selected)
{
	bool pe = false;
	switch (style)
	{
	case 0:
	{
		pe = Button0(font, font_size, icon, label, color_text, color_held, framesize, selected);
		break;
	}

	}
	return pe;
}

bool ImGuiUI::Button0(ImFont* font, float font_size, const char* icon, const char* label, ImColor color_text, ImColor color_held, ImVec2 framesize, bool selected)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = font->CalcTextSizeA(font_size, framesize.x, 0.0f, icon);
	ImVec2 pos = window->DC.CursorPos;
	const ImVec2 winSize = window->Size;

	//位置变化相关的动画
	{
		doFade(pos, label, winSize, g.MovingWindow == NULL);
	}


	//控件框架位置
	const ImRect frame = { pos , pos + framesize };
	//标签位置
	ImRect label_bb(frame.Min + ImVec2(frame.GetWidth() / 2.0f - label_size.x / 2.0f, frame.GetHeight() / 2.0f - label_size.y / 2.0f), { 0,0 });
	label_bb.Max = label_bb.Min + label_size;



	ItemSize(frame, style.FramePadding.y);
	if (!ItemAdd(frame, id))
	{
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
		return false;
	}

	bool hovered, held;
	bool pressed = ButtonBehavior(frame, id, &hovered, &held);



	RenderNavHighlight(label_bb, id);

	//控件背景
	{

		string id_str = label;

		static std::map <string, ImVec4> anim;
		auto it_anim = anim.find(id_str);
		if (it_anim == anim.end())
		{
			anim.insert({ id_str, ImVec4(62, 208, 142,255) });
			it_anim = anim.find(id_str);
		}

		it_anim->second = ImLerp(it_anim->second, (hovered && held) ? ImVec4(59, 206, 140, 255) : hovered ? ImVec4(59, 206, 140, 255) : ImVec4(46, 178, 119, 255), ImGui::GetIO().DeltaTime / 0.25f);
		ImU32 frame_bg = ImColor(it_anim->second / 255.0f);

		draw_list->AddRectFilled(frame.Min, frame.Max, frame_bg, 10.0f);
	}


	if (label_size.x > 0.0f)
	{
		//RenderText(label_bb.Min, icon);
		ImU32 label_col = held ? color_text : color_held;

		draw_list->AddText(font, font_size, label_bb.Min, label_col, icon);
		if (selected)
		{

			ImVec2 selectedbox_size = { 4.7f, frame.GetHeight() * 0.5f };
			ImRect selectedbox_bb(frame.Min + ImVec2(frame.GetWidth() - selectedbox_size.x, frame.GetHeight() / 2.0f - selectedbox_size.y / 2.0f), { 0,0 });
			selectedbox_bb.Max = selectedbox_bb.Min + selectedbox_size;
			static std::map <ImGuiWindow*, ImRect> anim_selectedbox;
			auto it_anim = anim_selectedbox.find(window);
			if (it_anim == anim_selectedbox.end())
			{
				anim_selectedbox.insert({ window,selectedbox_bb });
				it_anim = anim_selectedbox.find(window);
			}
			it_anim->second.Min = ImLerp(it_anim->second.Min, selectedbox_bb.Min, ImGui::GetIO().DeltaTime / (it_anim->second.Min.y > selectedbox_bb.Min.y ? 0.05f : 0.1f));
			it_anim->second.Max = ImLerp(it_anim->second.Max, selectedbox_bb.Max, ImGui::GetIO().DeltaTime / (it_anim->second.Min.y > selectedbox_bb.Min.y ? 0.1f : 0.05f));
			if (g.MovingWindow != NULL)
				it_anim->second = selectedbox_bb;
			draw_list->AddRectFilled(it_anim->second.Min, it_anim->second.Max, ImColor(80, 200, 160, 255), 9.0f, ImDrawFlags_RoundCornersAll);
		}

	}


	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
	return pressed;
}
bool ImGuiUI::Button(ImFont* font, float font_size, const char* icon, const char* label, ImColor color_text, ImColor color_held, ImVec2 framesize, bool selected)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = font->CalcTextSizeA(font_size, framesize.x, 0.0f, icon);
	ImVec2 pos = window->DC.CursorPos;
	const ImVec2 winSize = window->Size;

	//位置变化相关的动画
	{
		doFade(pos, label, winSize, g.MovingWindow == NULL);
	}


	//控件框架位置
	const ImRect frame = { pos , pos + framesize };
	//标签位置
	ImRect label_bb(frame.Min + ImVec2(frame.GetWidth() / 2.0f - label_size.x / 2.0f, frame.GetHeight() / 2.0f - label_size.y / 2.0f), { 0,0 });
	label_bb.Max = label_bb.Min + label_size;



	ItemSize(frame, style.FramePadding.y);
	if (!ItemAdd(frame, id))
	{
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
		return false;
	}

	bool hovered, held;
	bool pressed = ButtonBehavior(frame, id, &hovered, &held);



	RenderNavHighlight(label_bb, id);

	//控件背景
	{

		string id_str = label;

		static std::map <string, ImVec4> anim;
		auto it_anim = anim.find(id_str);
		if (it_anim == anim.end())
		{
			anim.insert({ id_str, ImVec4(255, 255, 255, 25) });
			it_anim = anim.find(id_str);
		}

		it_anim->second = ImLerp(it_anim->second, (hovered && held) ? ImVec4(200, 200, 200, 75) : hovered ? ImVec4(200, 200, 200, 50) : ImVec4(255, 255, 255, 0), ImGui::GetIO().DeltaTime / 0.25f);
		ImU32 frame_bg = ImColor(it_anim->second / 255.0f);

		draw_list->AddRectFilled(frame.Min, frame.Max, frame_bg);
	}


	if (label_size.x > 0.0f)
	{
		//RenderText(label_bb.Min, icon);
		ImU32 label_col = held ? color_held : color_text;

		draw_list->AddText(font, font_size, label_bb.Min, label_col, icon);
		if (selected)
		{

			ImVec2 selectedbox_size = { 4.7f, frame.GetHeight() * 0.5f };
			ImRect selectedbox_bb(frame.Min + ImVec2(frame.GetWidth() - selectedbox_size.x, frame.GetHeight() / 2.0f - selectedbox_size.y / 2.0f), { 0,0 });
			selectedbox_bb.Max = selectedbox_bb.Min + selectedbox_size;
			static std::map <ImGuiWindow*, ImRect> anim_selectedbox;
			auto it_anim = anim_selectedbox.find(window);
			if (it_anim == anim_selectedbox.end())
			{
				anim_selectedbox.insert({ window,selectedbox_bb });
				it_anim = anim_selectedbox.find(window);
			}
			it_anim->second.Min = ImLerp(it_anim->second.Min, selectedbox_bb.Min, ImGui::GetIO().DeltaTime / (it_anim->second.Min.y > selectedbox_bb.Min.y ? 0.05f : 0.1f));
			it_anim->second.Max = ImLerp(it_anim->second.Max, selectedbox_bb.Max, ImGui::GetIO().DeltaTime / (it_anim->second.Min.y > selectedbox_bb.Min.y ? 0.1f : 0.05f));
			if (g.MovingWindow != NULL)
				it_anim->second = selectedbox_bb;
			draw_list->AddRectFilled(it_anim->second.Min, it_anim->second.Max, ImColor(80, 200, 160, 255), 9.0f, ImDrawFlags_RoundCornersAll);
		}

	}


	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
	return pressed;
}

void ImGuiUI::setNextSubWidget()
{
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	SetCursorPos(GetCursorPos() - ImVec2(0, style.ItemSpacing.y * 0.9f));
}



ImVec2 ImGuiUI::CalcTextSize_ex(ImFont* font_Cale, const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width)
{
	ImGuiContext& g = *GImGui;

	const char* text_display_end;
	if (hide_text_after_double_hash)
		text_display_end = FindRenderedTextEnd(text, text_end);      // Hide anything after a '##' string
	else
		text_display_end = text_end;

	ImFont* font = font_Cale;
	const float font_size = g.FontSize;
	if (text == text_display_end)
		return ImVec2(0.0f, font_size);
	ImVec2 text_size = font->CalcTextSizeA(font_size, FLT_MAX, wrap_width, text, text_display_end, NULL);

	// Round
	// FIXME: This has been here since Dec 2015 (7b0bf230) but down the line we want this out.
	// FIXME: Investigate using ceilf or e.g.
	// - https://git.musl-libc.org/cgit/musl/tree/src/math/ceilf.c
	// - https://embarkstudios.github.io/rust-gpu/api/src/libm/math/ceilf.rs.html
	text_size.x = IM_TRUNC(text_size.x + 0.99999f);

	return text_size;
}


bool ImGuiUI::Text(const char* label, const char* anim_id, float wra, int sty)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true, wra);
	ImVec2 pos = window->DC.CursorPos;
	const ImVec2 winSize = window->Size;


	{

		doFade(pos, anim_id == 0 ? label : anim_id, winSize, g.MovingWindow == NULL);

	}
	//控件框架位置
	const ImRect frame = { pos , pos + label_size + ImVec2(10.0f,10.0f) * 2.0f };

	//标签位置
	ImRect label_bb(frame.Min + ImVec2(10.0f, 10.0f), { 0,0 });
	label_bb.Max = label_bb.Min + label_size;



	ItemSize(frame, style.FramePadding.y);
	if (!ItemAdd(frame, id))
	{
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
		return false;
	}



	RenderNavHighlight(label_bb, id);

	//控件背景
	{
		bool hovered = ItemHoverable(frame, id, 0);

		string id_str;
		if (anim_id != 0)
			id_str = anim_id;
		else
			id_str = label;
		static std::map <string, ImVec4> anim;
		auto it_anim = anim.find(id_str);
		if (it_anim == anim.end())
		{
			anim.insert({ id_str, ImVec4(255, 255, 255, 25) });
			it_anim = anim.find(id_str);
		}

		switch (sty)
		{
		case 0:
		{

			it_anim->second = ImLerp(it_anim->second, hovered ? ImVec4(218, 248, 212, 255) : ImVec4(218, 248, 212, 240), ImGui::GetIO().DeltaTime / 0.25f);

			break;
		}
		case 1:
		{

			it_anim->second = ImLerp(it_anim->second, hovered ? ImVec4(242, 242, 242, 255) : ImVec4(242, 242, 242, 240), ImGui::GetIO().DeltaTime / 0.25f);

			break;
		}

		}
		const ImU32 frame_bg = ImColor(it_anim->second / 255.0f);
		window->DrawList->AddRectFilled(frame.Min, frame.Max, frame_bg, 5.0f);
	}



	if (label_size.x > 0.0f)
		RenderTextWrapped(label_bb.Min, label, 0, wra);


	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
	return true;
}
bool ImGuiUI::imageSelectable(ImTextureID image,ImFont* font, const char* icon, float fontsize, ImVec2 framesize, void* p_data, const void* p_min, const void* p_max,const char* format, bool selected)
{

	//draw->AddImageRounded(image_user_img, point + ImVec2(20.0f, 9.0f), point + ImVec2(20.0f, 9.0f) + ImVec2(45, 45), { 0,0 }, { 1,1 }, ImColor(255, 255, 255), 50.0f);
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(icon);
	const ImVec2 label_size = font->CalcTextSizeA(fontsize, framesize.x, 0.0f, icon);
	ImVec2 pos = window->DC.CursorPos;
	const ImVec2 winSize = window->Size;

	//位置变化相关的动画
	{
		doFade(pos, icon, winSize, g.MovingWindow == NULL);
	}


	//控件框架位置
	const ImRect frame = { pos , pos + framesize };

	//图片位置
	ImVec2 image_size = { frame.GetHeight() * 0.8f,	frame.GetHeight() * 0.8f };
	ImRect image_bb = { {frame.Min.x + 20,frame.Min.y + frame.GetHeight() * 0.5f - image_size.y * 0.5f} , {} };
	image_bb.Max = image_bb.Min + image_size;

	//标签位置
	ImRect label_bb( ImVec2(image_bb.Max.x +10.0f, frame.Min.y+frame.GetHeight() / 2.0f - label_size.y / 2.0f), { 0,0 });
	label_bb.Max = label_bb.Min + label_size;

	const ImVec2 grab_size = ImVec2(180.0f, 8.0f);

	//滑块位置
	ImRect grab_bb(frame.Min + ImVec2(frame.GetWidth() - 5 - grab_size.x, frame.GetHeight() / 2.0f - grab_size.y / 2.0f), { 0,0 });
	if (framesize.x > 420.0f)
		grab_bb.Min.x -= 15.0f;
	grab_bb.Max = grab_bb.Min + grab_size;

	 

	ItemSize(frame, style.FramePadding.y);
	if (!ItemAdd(frame, id))
	{
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
		return false;
	}

	// Default format string when passing NULL
	if (format == NULL)
		format = DataTypeGetInfo(ImGuiDataType_Float)->PrintFmt;
	else if (ImGuiDataType_Float == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
		format = PatchFormatStringFloatToInt(format);

	bool hovered, held;
	bool pressed = ButtonBehavior(frame, id, &hovered, &held);


	// Tabbing or CTRL-clicking on Slider turns it into an input box
	const bool hovered_g = ItemHoverable(grab_bb, id, 0);
	const bool clicked_g = (hovered_g && g.IO.MouseClicked[0]);
	if (clicked_g)
	{
		SetActiveID(id, window);
		SetFocusID(id, window);
		FocusWindow(window);
		g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
	}



	static std::map <ImGuiID, slider_element> anim;
	auto it_anim = anim.find(id);
	if (it_anim == anim.end())
	{
		anim.insert({ id, { 0.0f } });
		it_anim = anim.find(id);
	}

	it_anim->second.opacity = ImLerp(it_anim->second.opacity, IsItemActive() ? 1.0f : 0.4f, 0.08f * (1.0f - ImGui::GetIO().DeltaTime));

	ImRect grab_bb_t;
	bool value_changed;
	{
		ImRect frame_bb_tem = grab_bb;

		frame_bb_tem.Max = frame_bb_tem.Max + ImVec2(12, 0);

		value_changed = SliderBehavior(frame_bb_tem, id, ImGuiDataType_Float, p_data, p_min, p_max, format, ImGuiSliderFlags_None, &grab_bb_t);
		if (value_changed)
			MarkItemEdited(id);
	}


	char value_buf[64];
	const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), ImGuiDataType_Float, p_data, format);

	


	//背景条
	window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, ImColor(15, 15, 16, 180), 5.0f);
	//值条
	window->DrawList->AddRectFilled(grab_bb.Min, ImVec2(grab_bb_t.Min.x + 3, grab_bb.Max.y), ImColor(147, 190, 66), 5.0f);
	//球
	window->DrawList->AddCircleFilled(ImVec2(grab_bb_t.Min.x - 1, grab_bb_t.Min.y + 2), 6.0f, ImColor(1.0f, 1.0f, 1.0f), 30);


	PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, it_anim->second.opacity)); {
		if(framesize.x>420.0f)
		RenderTextClipped(label_bb.Min, {grab_bb.Min.x-20.0f ,frame.Max.y}, value_buf, value_buf_end, NULL, ImVec2(1.f, 0.f));
		else
		RenderTextClipped({ label_bb.Min .x,label_bb.Min .y-18.0f}, {grab_bb.Min.x+30.0f ,frame.Max.y}, value_buf, value_buf_end, NULL, ImVec2(1.f, 0.f));
	} PopStyleColor();

	RenderNavHighlight(label_bb, id);

	//控件背景
	{

		string id_str = icon;

		static std::map <string, ImVec4> anim;
		auto it_anim = anim.find(id_str);
		if (it_anim == anim.end())
		{
			anim.insert({ id_str, ImVec4(255, 255, 255, 100) });
			it_anim = anim.find(id_str);
		}

		it_anim->second = ImLerp(it_anim->second, (hovered && held) ? ImVec4(200, 200, 200, 150) : hovered ? ImVec4(200, 200, 200, 125) : ImVec4(255, 255, 255, 0), ImGui::GetIO().DeltaTime / 0.25f);
		ImU32 frame_bg = ImColor(it_anim->second / 255.0f);

		window->DrawList ->AddRectFilled(frame.Min, frame.Max, frame_bg);
	}


	if (label_size.x > 0.0f)
	{
		//RenderText(label_bb.Min, icon);
		ImU32 label_col = selected ? ImColor(80, 200, 160, 255) : ImColor(200, 200, 200, 255);

		window->DrawList->AddText(font, fontsize, label_bb.Min, ImColor(255, 255, 255, 255), icon);
		window->DrawList->AddImageRounded(image, image_bb.Min, image_bb.Max, { 0,0 }, { 1,1 }, ImColor(255, 255, 255), 50.0f);
		if (selected)
		{

			ImVec2 selectedbox_size = { 4.7f, frame.GetHeight() * 0.5f };
			ImRect selectedbox_bb(frame.Min + ImVec2(frame.GetWidth() - selectedbox_size.x, frame.GetHeight() / 2.0f - selectedbox_size.y / 2.0f), { 0,0 });
			selectedbox_bb.Max = selectedbox_bb.Min + selectedbox_size;
			static std::map <ImGuiWindow*, ImRect> anim_selectedbox;
			auto it_anim = anim_selectedbox.find(window);
			if (it_anim == anim_selectedbox.end())
			{
				anim_selectedbox.insert({ window,selectedbox_bb });
				it_anim = anim_selectedbox.find(window);
			}
			it_anim->second.Min = ImLerp(it_anim->second.Min, selectedbox_bb.Min, ImGui::GetIO().DeltaTime / (it_anim->second.Min.y > selectedbox_bb.Min.y ? 0.05f : 0.1f));
			it_anim->second.Max = ImLerp(it_anim->second.Max, selectedbox_bb.Max, ImGui::GetIO().DeltaTime / (it_anim->second.Min.y > selectedbox_bb.Min.y ? 0.1f : 0.05f));
			if (g.MovingWindow != NULL)
				it_anim->second = selectedbox_bb;
			window->DrawList->AddRectFilled(it_anim->second.Min, it_anim->second.Max, ImColor(80, 200, 160, 255), 9.0f, ImDrawFlags_RoundCornersAll);
		}

	}


	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
	return pressed;
}
bool ImGuiUI::iconSelectable(ImFont* font, const char* icon, float fontsize, ImVec2 framesize, bool selected)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(icon);
	const ImVec2 label_size = font->CalcTextSizeA(fontsize, framesize.x, 0.0f, icon);
	ImVec2 pos = window->DC.CursorPos;
	const ImVec2 winSize = window->Size;

	//位置变化相关的动画
	{
		doFade(pos, icon, winSize, g.MovingWindow == NULL);
	}


	//控件框架位置
	const ImRect frame = { pos , pos + framesize };
	//标签位置
	ImRect label_bb(frame.Min + ImVec2(frame.GetWidth() / 2.0f - label_size.x / 2.0f, frame.GetHeight() / 2.0f - label_size.y / 2.0f), { 0,0 });
	label_bb.Max = label_bb.Min + label_size;



	ItemSize(frame, style.FramePadding.y);
	if (!ItemAdd(frame, id))
	{
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
		return false;
	}

	bool hovered, held;
	bool pressed = ButtonBehavior(frame, id, &hovered, &held);



	RenderNavHighlight(label_bb, id);

	//控件背景
	{

		string id_str = icon;

		static std::map <string, ImVec4> anim;
		auto it_anim = anim.find(id_str);
		if (it_anim == anim.end())
		{
			anim.insert({ id_str, ImVec4(255, 255, 255, 25) });
			it_anim = anim.find(id_str);
		}

		it_anim->second = ImLerp(it_anim->second, (hovered && held) ? ImVec4(200, 200, 200, 75) : hovered ? ImVec4(200, 200, 200, 50) : ImVec4(255, 255, 255, 0), ImGui::GetIO().DeltaTime / 0.25f);
		ImU32 frame_bg = ImColor(it_anim->second / 255.0f);

		draw_list->AddRectFilled(frame.Min, frame.Max, frame_bg);
	}


	if (label_size.x > 0.0f)
	{
		//RenderText(label_bb.Min, icon);
		ImU32 label_col = selected ? ImColor(80, 200, 160, 255) : ImColor(200, 200, 200, 255);

		draw_list->AddText(font, fontsize, label_bb.Min, label_col, icon);
		if (selected)
		{

			ImVec2 selectedbox_size = { 4.7f, frame.GetHeight() * 0.5f };
			ImRect selectedbox_bb(frame.Min + ImVec2(frame.GetWidth() - selectedbox_size.x, frame.GetHeight() / 2.0f - selectedbox_size.y / 2.0f), { 0,0 });
			selectedbox_bb.Max = selectedbox_bb.Min + selectedbox_size;
			static std::map <ImGuiWindow*, ImRect> anim_selectedbox;
			auto it_anim = anim_selectedbox.find(window);
			if (it_anim == anim_selectedbox.end())
			{
				anim_selectedbox.insert({ window,selectedbox_bb });
				it_anim = anim_selectedbox.find(window);
			}
			it_anim->second.Min = ImLerp(it_anim->second.Min, selectedbox_bb.Min, ImGui::GetIO().DeltaTime / (it_anim->second.Min.y > selectedbox_bb.Min.y ? 0.05f : 0.1f));
			it_anim->second.Max = ImLerp(it_anim->second.Max, selectedbox_bb.Max, ImGui::GetIO().DeltaTime / (it_anim->second.Min.y > selectedbox_bb.Min.y ? 0.1f : 0.05f));
			if (g.MovingWindow != NULL)
				it_anim->second = selectedbox_bb;
			draw_list->AddRectFilled(it_anim->second.Min, it_anim->second.Max, ImColor(80, 200, 160, 255), 9.0f, ImDrawFlags_RoundCornersAll);
		}

	}


	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
	return pressed;
}
bool ImGuiUI::iconSelectable(ImFont* font, const char* icon, const char* label, float fontsize, ImVec2 framesize, bool selected)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = font->CalcTextSizeA(fontsize, framesize.x, 0.0f, icon);
	ImVec2 pos = window->DC.CursorPos;
	const ImVec2 winSize = window->Size;

	//位置变化相关的动画
	{
		doFade(pos, label, winSize, g.MovingWindow == NULL);
	}


	//控件框架位置
	const ImRect frame = { pos , pos + framesize };
	//标签位置
	ImRect label_bb(frame.Min + ImVec2(frame.GetWidth() / 2.0f - label_size.x / 2.0f, frame.GetHeight() / 2.0f - label_size.y / 2.0f), { 0,0 });
	label_bb.Max = label_bb.Min + label_size;



	ItemSize(frame, style.FramePadding.y);
	if (!ItemAdd(frame, id))
	{
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
		return false;
	}

	bool hovered, held;
	bool pressed = ButtonBehavior(frame, id, &hovered, &held);



	RenderNavHighlight(label_bb, id);

	//控件背景
	{

		string id_str = label;

		static std::map <string, ImVec4> anim;
		auto it_anim = anim.find(id_str);
		if (it_anim == anim.end())
		{
			anim.insert({ id_str, ImVec4(255, 255, 255, 25) });
			it_anim = anim.find(id_str);
		}

		it_anim->second = ImLerp(it_anim->second, (hovered && held) ? ImVec4(200, 200, 200, 75) : hovered ? ImVec4(200, 200, 200, 50) : ImVec4(255, 255, 255, 0), ImGui::GetIO().DeltaTime / 0.25f);
		ImU32 frame_bg = ImColor(it_anim->second / 255.0f);

		draw_list->AddRectFilled(frame.Min, frame.Max, frame_bg);
	}


	if (label_size.x > 0.0f)
	{
		//RenderText(label_bb.Min, icon);
		ImU32 label_col = selected ? ImColor(80, 200, 160, 255) : ImColor(235, 235, 235);

		draw_list->AddText(font, fontsize, label_bb.Min, label_col, icon);
		if (false)
		{

			ImVec2 selectedbox_size = { 4.7f, frame.GetHeight() * 0.5f };
			ImRect selectedbox_bb(frame.Min + ImVec2(frame.GetWidth() - selectedbox_size.x, frame.GetHeight() / 2.0f - selectedbox_size.y / 2.0f), { 0,0 });
			selectedbox_bb.Max = selectedbox_bb.Min + selectedbox_size;
			static std::map <ImGuiWindow*, ImRect> anim_selectedbox;
			auto it_anim = anim_selectedbox.find(window);
			if (it_anim == anim_selectedbox.end())
			{
				anim_selectedbox.insert({ window,selectedbox_bb });
				it_anim = anim_selectedbox.find(window);
			}
			it_anim->second.Min = ImLerp(it_anim->second.Min, selectedbox_bb.Min, ImGui::GetIO().DeltaTime / (it_anim->second.Min.y > selectedbox_bb.Min.y ? 0.05f : 0.1f));
			it_anim->second.Max = ImLerp(it_anim->second.Max, selectedbox_bb.Max, ImGui::GetIO().DeltaTime / (it_anim->second.Min.y > selectedbox_bb.Min.y ? 0.1f : 0.05f));
			if (g.MovingWindow != NULL)
				it_anim->second = selectedbox_bb;
			draw_list->AddRectFilled(it_anim->second.Min, it_anim->second.Max, ImColor(80, 200, 160, 255), 9.0f, ImDrawFlags_RoundCornersAll);
		}

	}


	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
	return pressed;
}

bool ImGuiUI::Selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	// Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
	ImGuiID id = window->GetID(label);
	ImVec2 label_size = CalcTextSize(label, NULL, true);
	ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
	ImVec2 pos = window->DC.CursorPos;
	pos.y += window->DC.CurrLineTextBaseOffset;
	ItemSize(size, 0.0f);

	// Fill horizontal space
	// We don't support (size < 0.0f) in Selectable() because the ItemSpacing extension would make explicitly right-aligned sizes not visibly match other widgets.
	const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;
	const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
	const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;
	if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
		size.x = ImMax(label_size.x, max_x - min_x);

	// Text stays at the submission position, but bounding box may be extended on both sides
	const ImVec2 text_min = pos;
	const ImVec2 text_max(min_x + size.x, pos.y + size.y);

	// Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
	ImRect bb(min_x, pos.y, text_max.x, text_max.y);
	if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
	{
		const float spacing_x = span_all_columns ? 0.0f : style.ItemSpacing.x;
		const float spacing_y = style.ItemSpacing.y;
		const float spacing_L = IM_TRUNC(spacing_x * 0.50f);
		const float spacing_U = IM_TRUNC(spacing_y * 0.50f);
		bb.Min.x -= spacing_L;
		bb.Min.y -= spacing_U;
		bb.Max.x += (spacing_x - spacing_L);
		bb.Max.y += (spacing_y - spacing_U);
	}
	//if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(0, 255, 0, 255)); }

	// Modify ClipRect for the ItemAdd(), faster than doing a PushColumnsBackground/PushTableBackgroundChannel for every Selectable..
	const float backup_clip_rect_min_x = window->ClipRect.Min.x;
	const float backup_clip_rect_max_x = window->ClipRect.Max.x;
	if (span_all_columns)
	{
		window->ClipRect.Min.x = window->ParentWorkRect.Min.x;
		window->ClipRect.Max.x = window->ParentWorkRect.Max.x;
	}

	const bool disabled_item = (flags & ImGuiSelectableFlags_Disabled) != 0;
	const bool item_add = ItemAdd(bb, id, NULL, disabled_item ? ImGuiItemFlags_Disabled : ImGuiItemFlags_None);
	if (span_all_columns)
	{
		window->ClipRect.Min.x = backup_clip_rect_min_x;
		window->ClipRect.Max.x = backup_clip_rect_max_x;
	}

	if (!item_add)
		return false;

	const bool disabled_global = (g.CurrentItemFlags & ImGuiItemFlags_Disabled) != 0;
	if (disabled_item && !disabled_global) // Only testing this as an optimization
		BeginDisabled();

	// FIXME: We can standardize the behavior of those two, we could also keep the fast path of override ClipRect + full push on render only,
	// which would be advantageous since most selectable are not selected.
	if (span_all_columns && window->DC.CurrentColumns)
		PushColumnsBackground();
	else if (span_all_columns && g.CurrentTable)
		TablePushBackgroundChannel();

	// We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
	ImGuiButtonFlags button_flags = 0;
	if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
	if (flags & ImGuiSelectableFlags_NoSetKeyOwner) { button_flags |= ImGuiButtonFlags_NoSetKeyOwner; }
	if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
	if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
	if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
	if ((flags & ImGuiSelectableFlags_AllowOverlap) || (g.LastItemData.InFlags & ImGuiItemFlags_AllowOverlap)) { button_flags |= ImGuiButtonFlags_AllowOverlap; }

	const bool was_selected = selected;
	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, button_flags);

	// Auto-select when moved into
	// - This will be more fully fleshed in the range-select branch
	// - This is not exposed as it won't nicely work with some user side handling of shift/control
	// - We cannot do 'if (g.NavJustMovedToId != id) { selected = false; pressed = was_selected; }' for two reasons
	//   - (1) it would require focus scope to be set, need exposing PushFocusScope() or equivalent (e.g. BeginSelection() calling PushFocusScope())
	//   - (2) usage will fail with clipped items
	//   The multi-select API aim to fix those issues, e.g. may be replaced with a BeginSelection() API.
	if ((flags & ImGuiSelectableFlags_SelectOnNav) && g.NavJustMovedToId != 0 && g.NavJustMovedToFocusScopeId == g.CurrentFocusScopeId)
		if (g.NavJustMovedToId == id)
			selected = pressed = true;

	// Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
	if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
	{
		if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
		{
			SetNavID(id, window->DC.NavLayerCurrent, g.CurrentFocusScopeId, WindowRectAbsToRel(window, bb)); // (bb == NavRect)
			g.NavDisableHighlight = true;
		}
	}
	if (pressed)
		MarkItemEdited(id);

	// In this branch, Selectable() cannot toggle the selection so this will never trigger.
	if (selected != was_selected) //-V547
		g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

	// Render
	if (1 || hovered || selected)
	{
		static std::map <ImGuiID, ImVec4> anim;
		auto it_anim = anim.find(id);
		if (it_anim == anim.end())
		{
			anim.insert({ id, ImVec4(80, 200, 160, 0) });
			it_anim = anim.find(id);
		}

		it_anim->second = ImLerp(it_anim->second, (held && hovered) ? ImVec4(80, 200, 160, 75) : (hovered || selected) ? ImVec4(80, 200, 160, 50) : ImVec4(80, 200, 160, 0), ImGui::GetIO().DeltaTime / 0.2f);

		ImU32 col = ImColor(it_anim->second / 255.0f);

		//const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
		RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
	}
	if (g.NavId == id)
		RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);

	if (span_all_columns && window->DC.CurrentColumns)
		PopColumnsBackground();
	else if (span_all_columns && g.CurrentTable)
		TablePopBackgroundChannel();

	RenderTextClipped(text_min, text_max, label, NULL, &label_size, style.SelectableTextAlign, &bb);

	// Automatically close popups
	if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(g.LastItemData.InFlags & ImGuiItemFlags_SelectableDontClosePopup))
		CloseCurrentPopup();

	if (disabled_item && !disabled_global)
		EndDisabled();

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	return pressed; //-V1020
}

bool ImGuiUI::beginChild(const char* name, ImVec2 pos, ImVec2 size)
{

	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 5.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));


	return ImGui::Begin(name, 0, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDecoration);

}

void ImGuiUI::endChild()
{
	ImGui::EndChild();
	ImGui::PopStyleVar(2);
}

void ImGuiUI::doFade(ImVec2& pos, const char* id, ImVec2 winSize, bool canDoFade)
{
	string id_str = id;
	auto it_anim = anim_move_iconSelectable.find(id_str);
	if (it_anim == anim_move_iconSelectable.end())
	{
		anim_move_iconSelectable.insert({ id_str,ImVec2(pos.x + winSize.x * 0.5f,pos.y) });
		it_anim = anim_move_iconSelectable.find(id_str);
	}
	if (canDoFade)
		it_anim->second = ImLerp(it_anim->second, pos, ImGui::GetIO().DeltaTime / anim_move_t);
	else
		it_anim->second = pos;
	pos = it_anim->second;

}



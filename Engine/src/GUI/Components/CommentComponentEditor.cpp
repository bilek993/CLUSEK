#include "CommentComponentEditor.h"
#include "../../Utils/ImGuiExtensions.h"

void CommentComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	ImGui::InputTextMultiline(	"Comment", 
								const_cast<char*>(component->Comment.c_str()), 
								component->Comment.capacity() + 1,
								ImVec2(0, 0),
								ImGuiInputTextFlags_CallbackResize,
								ImGuiExtensions::InputTextStringCallback,
								&component->Comment);
}

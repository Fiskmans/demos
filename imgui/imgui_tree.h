
#include "imgui.h"

#include <concepts>
#include <vector>

namespace imgui_extensions
{
	float operator ""_deg(unsigned long long aValue)
	{
		return aValue * 0.0174532f;
	}

	template<class NodeType, class DrawingFunction, class ChildGetter>
		requires
		requires(NodeType&& node, DrawingFunction&& drawing, ChildGetter&& childGetter)
	{
		{
			drawing(std::forward<NodeType>(node))
		};
		{
			*std::begin(childGetter(std::forward<NodeType>(node)))
		} -> std::convertible_to<NodeType&>;
	}
	ImVec2 Tree(const char* aStringId, NodeType&& aNode, DrawingFunction&& aDrawingFunction, ChildGetter&& aChildGetter, ImColor aColor = ImGui::GetStyle().Colors[ImGuiCol_PlotLines], float aIndentdepth = 15.f, float aRounding = 0, float aThickness = 1)
	{
		ImGui::PushID(aStringId);

		ImVec2 anchor;
		ImVec2 treeRoot;

		{
			ImVec2 start = ImGui::GetCursorScreenPos();
			ImGui::BeginChild("node", { 0, 0 }, ImGuiChildFlags_AutoResizeY);

			aDrawingFunction(std::forward<NodeType>(aNode));

			ImGui::EndChild();
			ImVec2 end = ImGui::GetCursorScreenPos();

			anchor = {
				start.x,
				(start.y + end.y) / 2.f
			};

			treeRoot = {
				end.x + 5.f,
				end.y
			};
		}

		ImGui::Indent(aIndentdepth);
		std::vector<ImVec2> anchors;
		for (NodeType& child : aChildGetter(std::forward<NodeType>(aNode)))
			anchors.push_back(Tree(std::to_string(anchors.size()).c_str(), child, aDrawingFunction, aChildGetter, aColor, aIndentdepth, aRounding, aThickness));

		ImGui::Unindent(aIndentdepth);

		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		for (ImVec2& anchor : anchors)
		{
			ImVec2 next;

			if (aRounding > 0.f)
			{
				float rounding = (std::min)(
					anchor.y - treeRoot.y,
					(std::min)(
						anchor.x - treeRoot.x,
						aRounding));

				next = {
					treeRoot.x,
					anchor.y - rounding
				};

				ImVec2 arcCenter = {
					treeRoot.x + rounding,
					anchor.y - rounding
				};

				ImVec2 arcEnd = {
					treeRoot.x + rounding,
					anchor.y
				};


				drawlist->PathLineTo(treeRoot);
				drawlist->PathLineTo(next);
				drawlist->PathArcTo(arcCenter, rounding, 180_deg, 90_deg);
				drawlist->PathLineTo(anchor);
			}
			else
			{
				next = {
					treeRoot.x,
					anchor.y
				};

				drawlist->PathLineTo(treeRoot);
				drawlist->PathLineTo(next);
				drawlist->PathLineTo(anchor);
			}

			drawlist->PathStroke(aColor, 0, aThickness);

			treeRoot = next;
		}

		ImGui::PopID();

		return anchor;
	}
}
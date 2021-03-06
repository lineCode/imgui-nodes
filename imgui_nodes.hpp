#pragma once

#include <imgui.h>
#include <vector>
#include <array>
#include <functional>

namespace nodes {

enum class UserAction {
    None, Delete, Copy, Cut, Paste, Undo, Redo, NewConnection
};

struct SlotState {
    int type;
    ImVec2 pos;
};

struct NodeState {
    NodeState(int id, ImVec2 initialPos);
    NodeState(NodeState const &other) = default;
    NodeState& operator= (const NodeState& other) = default;

    int id;

    ImVec2 pos;
    ImVec2 posFloat;
    ImVec2 size;

    ImVec2 lastCursor;

    std::vector<SlotState> inputSlots;
    std::vector<SlotState> outputSlots;
};

struct Grid {
    int spacingMultiplier;
    float thickness;
    ImColor lineColor;
};

struct Style {
    ImColor selectionFill;
    ImColor selectionBorder;

    float   connectorSize;
    ImColor connectorSelectedColor;
    float   connectorSelectedSize;
    ImColor connectorDragging;
    float   connectorDraggingSize;
    ImColor connectorHovered;
    ImColor connectorInvalid;

    ImVec2  nodePadding;
    ImColor nodeFill;
    ImColor nodeFillHovered;
    ImColor nodeBorder;
    ImColor nodeBorderSelected;
    float   nodeRounding;
    float   nodeBorderSize;

    float   slotRadius;
    float   slotMouseRadius;
    ImColor slotBorderHovered;

    std::vector<ImColor> connectorTypeColor;

    // todo: sort this vector by descending spacing
    std::vector<Grid> grid;
    float gridSpacing;

    Style() 
        : selectionFill(50, 50, 50, 50)
        , selectionBorder(255, 255, 255, 150)
        , connectorSize(3.f)
        , connectorSelectedColor(255, 128, 0)
        , connectorSelectedSize(4.f)
        , connectorDragging(255, 128, 0)
        , connectorDraggingSize(3.f)
        , connectorHovered(192, 192, 192)
        , connectorInvalid(255, 0, 0)
        , nodePadding(8.f, 8.f)
        , nodeFill(60, 60, 60)
        , nodeFillHovered(75, 75, 75)
        , nodeBorder(100, 100, 100)
        , nodeBorderSelected(255, 128, 0)
        , nodeRounding(4.f)
        , nodeBorderSize(2.f)
        , slotRadius(4.f)
        , slotMouseRadius(10.f)
        , slotBorderHovered(255, 128, 0)
        , connectorTypeColor({
            ImColor(150, 150, 250, 150),
            ImColor(250, 150, 250, 150),
            ImColor(250, 250, 150, 150),
            ImColor(150, 250, 250, 150) })
        , grid({
            Grid{ 64, 1.f, ImColor(150, 150, 150, 200) },
            Grid{ 16, 1.f, ImColor(200, 200, 200, 100) },
            Grid{ 1, 1.f, ImColor(230, 230, 230, 40) }})
        , gridSpacing(16)
    {}

    void generate();
};

struct NodeArea {
    enum class Mode { 
        None, 
        Selecting, SelectionCaptureAdd, SelectionCaptureRemove, 
        DraggingNodes, ResizingNode,
        DraggingConnectorInput, DraggingConnectorOutput,
        Escaped, SelectAll
    };

    struct Selection {
        int selectedCount = 0;
        std::vector<bool> selectedItems; // there should be a specialization for this type ...

        void clearSelection();
        void addToSelection(int id);
        void removeFromSelection(int id);
        void toggleSelection(int id);
        bool isSelected(int id) const;
    };

    struct InternalState {
        bool initialized = false;
        ImVec2 nodeAreaSize = ImVec2(20000.f, 20000.f);

        ImGuiContext* innerContext;
        ImGuiContext* outerContext;
        ImVector<ImDrawList*> renderDrawLists[3];

        float zoom;
        float snapGrid = 16.f;
        ImVec2 innerWndPos;

        Mode mode;
        bool scrolling;

        ImVec2 dragStart;
        ImVec2 dragEnd;

        int connectorStartNode;
        int connectorStartSlot;
        int connectorEndNode;
        int connectorEndSlot;

        int activeNode = -1;
        int hoveredNode = -1;
        int hoveredLink = -1;
        Selection selectedNodes;
        Selection selectedLinks;

        bool outerWindowFocused;
        bool outerWindowHovered;

        ImDrawList nodeDrawList;
        ImDrawList overlayDrawList;
        ImDrawList *windowDrawList;
    } state;

    Style style;

    void clearAllSelections() {
        state.selectedNodes.clearSelection();
        state.selectedLinks.clearSelection();
    }

    void BeginNodeArea(std::function<void(UserAction)> actionCallback, bool updateStyle = false);
    void EndNodeArea();

    bool BeginNode(NodeState &node);
    void EndNode(NodeState &node, bool resizable = false);

    void BeginSlot(NodeState &node);
    void EndSlot(NodeState &node, int inputType = -1, int outputType = -1);

    bool ConnectNodeSlots(int connectorId, NodeState &sourceNode, int sourceSlot, NodeState &sinkNode, int sinkSlot);

    bool GetNewConnection(int *connectorSourceNode, int *connectorSourceNodeSlot, int *connectorSinkNode, int *connectorSinkNodeSlot);
};

static_assert(std::is_default_constructible<NodeArea>::value, "");
static_assert(std::is_copy_constructible<NodeArea>::value, "");
static_assert(std::is_move_constructible<NodeArea>::value, "");
static_assert(std::is_copy_assignable<NodeArea>::value, "");
static_assert(std::is_move_assignable<NodeArea>::value, "");

} // namespace nodes

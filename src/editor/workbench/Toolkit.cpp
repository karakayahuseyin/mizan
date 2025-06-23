#include "Toolkit.h"
#include "brep/Builder.h"
#include "brep/Solid.h"
#include "renderer/Tessellator.h"
#include "logger/Logger.h"

#include <string>
#include <chrono>

Toolkit::Toolkit(Window* window, Scene* scene)
    : m_window(window), m_scene(scene), m_modeller(nullptr) {
    // Initialize default tessellation parameters
    m_tessellationParams = TessellationParams{};
    m_tessellationParams.method = TessellationMethod::CPU;
    m_tessellationParams.tessellationLevel = 1.0f;
    m_tessellationParams.smoothNormals = true;
}

Toolkit::~Toolkit() {
    // Destructor implementation
    if (m_modeller) {
        delete m_modeller; // Clean up the Modeller instance if it was created
    }
}

void Toolkit::initialize() {
    // Initialization code for the toolkit
}

void Toolkit::update() {
    // Update logic for the toolkit
}

void Toolkit::render() {
    m_window->beginImGuiFrame();

    // Main toolbar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Tools")) {
            if (ImGui::MenuItem("Tessellation Test", nullptr, m_showTessellationWindow)) {
                m_showTessellationWindow = !m_showTessellationWindow;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Primitive creation buttons
    ImGui::Begin("Primitive Tools");
    if (ImGui::Button("Add Cube")) {
        addSolid("Cube", BREP::Solid::PrimitiveType::Cube);
    }
    if (ImGui::Button("Add Sphere")) {
        addSolid("Sphere", BREP::Solid::PrimitiveType::Sphere);
    }
    if (ImGui::Button("Add Cylinder")) {
        addSolid("Cylinder", BREP::Solid::PrimitiveType::Cylinder);
    }
    if (ImGui::Button("Add Pyramid")) {
        addSolid("Pyramid", BREP::Solid::PrimitiveType::Pyramid);
    }
    ImGui::End();

    // Tessellation test window
    if (m_showTessellationWindow) {
        renderTessellationTestWindow();
    }
    
    m_window->endImGuiFrame();
}

void Toolkit::cleanup() {
    // Cleanup code for the toolkit
}

void Toolkit::addSolid(std::string name, BREP::Solid::PrimitiveType type) {
    if (!m_scene) {
        Logger::error("Scene is not initialized. Cannot add solid.");
        return;
    }

    SceneObject newObject;
    newObject.name = name;

    newObject.solid = BREP::Builder::createSolid(type);
    
    // Use default tessellation parameters for regular objects
    TessellationParams defaultParams;
    defaultParams.method = TessellationMethod::CPU;
    defaultParams.tessellationLevel = 1.0f;
    defaultParams.smoothNormals = true;
    
    newObject.mesh = Tessellator::tessellate(newObject.solid, defaultParams);

    if (newObject.mesh.getVertexCount() == 0) {
        Logger::error("Tessellation failed. Mesh is empty. Cannot add solid.");
        return;
    }

    newObject.mesh.setColor(0.75f, 0.75f, 0.75f);
    newObject.mesh.m_showWireframe = true;
    newObject.mesh.m_showSolid = true;

    if (m_scene->addObject(newObject)) {
        Logger::info("Solid added successfully.");
    } else {
        Logger::error("Failed to add solid.");
    }
}

void Toolkit::renderTessellationTestWindow() {
    ImGui::Begin("Tessellation Test", &m_showTessellationWindow);
    
    bool paramsChanged = false;
    
    // Primitive selection
    ImGui::Text("Test Primitive:");
    ImGui::SameLine();
    const char* primitives[] = { "Cube", "Sphere", "Cylinder", "Pyramid" };
    if (ImGui::Combo("##Primitive", &m_selectedPrimitive, primitives, 4)) {
        paramsChanged = true;
    }
    
    ImGui::Separator();
    
    // Tessellation method selection
    ImGui::Text("Tessellation Method:");
    int methodIndex = static_cast<int>(m_tessellationParams.method);
    const char* methods[] = { "CPU", "GPU", "Adaptive" };
    if (ImGui::Combo("##Method", &methodIndex, methods, 3)) {
        m_tessellationParams.method = static_cast<TessellationMethod>(methodIndex);
        paramsChanged = true;
    }
    
    ImGui::Separator();
    
    // Quality parameters
    ImGui::Text("Quality Parameters:");
    if (ImGui::SliderFloat("Tessellation Level", &m_tessellationParams.tessellationLevel, 0.1f, 10.0f, "%.2f")) {
        paramsChanged = true;
    }
    if (ImGui::SliderFloat("Curvature Threshold", &m_tessellationParams.curvatureThreshold, 0.01f, 1.0f, "%.3f")) {
        paramsChanged = true;
    }
    if (ImGui::SliderFloat("Edge Length Limit", &m_tessellationParams.edgeLengthLimit, 0.1f, 5.0f, "%.2f")) {
        paramsChanged = true;
    }
    
    ImGui::Separator();
    
    // GPU-specific parameters
    if (m_tessellationParams.method == TessellationMethod::GPU || 
        m_tessellationParams.method == TessellationMethod::ADAPTIVE) {
        ImGui::Text("GPU Parameters:");
        if (ImGui::SliderFloat("Inner Tess Level", &m_tessellationParams.innerTessLevel, 1.0f, 64.0f, "%.0f")) {
            paramsChanged = true;
        }
        if (ImGui::SliderFloat("Outer Tess Level", &m_tessellationParams.outerTessLevel, 1.0f, 64.0f, "%.0f")) {
            paramsChanged = true;
        }
        if (ImGui::Checkbox("Use Geometry Shader", &m_tessellationParams.useGeometryShader)) {
            paramsChanged = true;
        }
        ImGui::Separator();
    }
    
    // Performance parameters
    ImGui::Text("Performance Parameters:");
    if (ImGui::SliderInt("Max Vertices Per Patch", &m_tessellationParams.maxVerticesPerPatch, 3, 128)) {
        paramsChanged = true;
    }
    if (ImGui::Checkbox("Enable LOD", &m_tessellationParams.enableLOD)) {
        paramsChanged = true;
    }
    if (m_tessellationParams.enableLOD) {
        if (ImGui::SliderFloat("LOD Distance", &m_tessellationParams.lodDistance, 1.0f, 50.0f, "%.1f")) {
            paramsChanged = true;
        }
    }
    
    ImGui::Separator();
    
    // Surface parameters
    ImGui::Text("Surface Parameters:");
    if (ImGui::Checkbox("Smooth Normals", &m_tessellationParams.smoothNormals)) {
        paramsChanged = true;
    }
    if (ImGui::Checkbox("Preserve Sharp Edges", &m_tessellationParams.preserveSharpEdges)) {
        paramsChanged = true;
    }
    if (m_tessellationParams.preserveSharpEdges) {
        if (ImGui::SliderFloat("Sharp Edge Threshold", &m_tessellationParams.sharpEdgeThreshold, 0.0f, 180.0f, "%.1f°")) {
            paramsChanged = true;
        }
    }
    
    ImGui::Separator();
    
    // Debugging options
    ImGui::Text("Debug Options:");
    ImGui::Checkbox("Show Wireframe", &m_showWireframe);
    ImGui::Checkbox("Show Statistics", &m_showStatistics);
    
    // Auto-update toggle
    ImGui::Checkbox("Auto Update", &m_autoUpdate);
    ImGui::SameLine();
    
    // Manual update button
    if (ImGui::Button("Update Test Object") || (m_autoUpdate && paramsChanged)) {
        updateTestObject();
    }
    
    // Performance info
    ImGui::Separator();
    ImGui::Text("Performance Info:");
    
    // Calculate complexity for current settings
    BREP::Solid::PrimitiveType primitiveType = static_cast<BREP::Solid::PrimitiveType>(m_selectedPrimitive);
    BREP::Solid testSolid = BREP::Builder::createSolid(primitiveType);
    float complexity = Tessellator::calculateComplexity(testSolid);
    TessellationMethod recommendedMethod = Tessellator::chooseBestMethod(testSolid, m_tessellationParams);
    
    ImGui::Text("Geometry Complexity: %.1f", complexity);
    ImGui::Text("Recommended Method: %s", 
                recommendedMethod == TessellationMethod::CPU ? "CPU" :
                recommendedMethod == TessellationMethod::GPU ? "GPU" : "Adaptive");
    
    // Show current object info if available
    if (m_scene && !m_scene->getSceneObjects().empty()) {
        const auto& lastObject = m_scene->getSceneObjects().back();
        ImGui::Text("Current Object:");
        ImGui::Text("  Vertices: %zu", lastObject.mesh.getVertexCount());
        ImGui::Text("  Triangles: %zu", lastObject.mesh.getTriangleCount());
    }
    
    // GPU status info
    ImGui::Separator();
    ImGui::Text("GPU Tessellation Status:");
    bool gpuInitialized = Tessellator::isGPUInitialized();
    if (gpuInitialized) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ GPU Tessellation Available");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "⚠ GPU Tessellation Not Available");
        if (ImGui::Button("Initialize GPU Tessellation")) {
            Tessellator::initializeGPUTessellation();
        }
    }
    
    ImGui::End();
}

void Toolkit::updateTestObject() {
    if (!m_scene) {
        Logger::error("Scene is not initialized. Cannot update test object.");
        return;
    }
    
    // Remove existing test object if it exists
    auto& objects = m_scene->getSceneObjects();
    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (it->name.find("TessellationTest_") == 0) {
            // Note: This is a const reference, we need to use scene's remove method
            // For now, just add the new object (scene will handle duplicates)
            break;
        }
    }
    
    // Create new test object
    SceneObject testObject;
    const char* primitiveNames[] = { "Cube", "Sphere", "Cylinder", "Pyramid" };
    testObject.name = std::string("TessellationTest_") + primitiveNames[m_selectedPrimitive];
    
    BREP::Solid::PrimitiveType primitiveType = static_cast<BREP::Solid::PrimitiveType>(m_selectedPrimitive);
    testObject.solid = BREP::Builder::createSolid(primitiveType);
    
    // Tessellate with current parameters
    auto startTime = std::chrono::high_resolution_clock::now();
    testObject.mesh = Tessellator::tessellate(testObject.solid, m_tessellationParams);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    std::string message = "Tessellation completed in " + std::to_string(duration.count()) + " microseconds";
    Logger::info(message);
    
    if (testObject.mesh.getVertexCount() == 0) {
        Logger::error("Tessellation failed. Mesh is empty.");
        return;
    }
    
    // Configure mesh appearance
    testObject.mesh.setColor(0.8f, 0.6f, 0.2f); // Orange color for test objects
    testObject.mesh.m_showWireframe = true;
    testObject.mesh.m_showSolid = true;
    
    if (m_scene->addObject(testObject)) {
        std::string message = "Test object updated successfully. Vertices: " + 
                             std::to_string(testObject.mesh.getVertexCount()) + 
                             ", Triangles: " + std::to_string(testObject.mesh.getTriangleCount());
        Logger::info(message);
    } else {
        Logger::error("Failed to add test object.");
    }
}
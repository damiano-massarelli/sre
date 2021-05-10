#include "Test.h"

#include "../src/Engine.h"

#include "imgui/imgui.h"

/**
 *  Force to run on dedicated graphic cards.
 */
#include <windows.h>
extern "C" {
_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

namespace sre {

TestSceneRegisterer::TestSceneRegisterer(std::string name, std::unique_ptr<ITestScene> test) {
    TestSceneManager::get()->registerTest(name, std::move(test));
}

std::unique_ptr<TestSceneManager> TestSceneManager::instance = nullptr;

TestSceneManager* TestSceneManager::get() {
    if (instance == nullptr) {
        instance = std::make_unique<TestSceneManager>();
    }

    return instance.get();
}

void TestSceneManager::registerTest(std::string name, std::unique_ptr<ITestScene> test) {
    // TODO: check that the test doesn't already exists
    mTests[name] = std::move(test);
}

void TestSceneManager::startTests() {
    Engine::uiRenderer.setDebugUIDrawer([this]() {
        ImGui::Begin("Tests");
        ImGui::SetWindowPos(ImVec2::ImVec2(), ImGuiCond_Once);
        ImGui::SetWindowSize("Tests", ImVec2::ImVec2(150, 500), ImGuiCond_Once);

        for (auto& testEntry : mTests) {
            if (ImGui::Button(testEntry.first.c_str())) {

                if (mCurrentTest != nullptr) {
                    mCurrentTest->end();
                    Engine::cleanUp();
                }

                mCurrentTest = testEntry.second.get();
                testEntry.second->start();
            }
        }

        ImGui::Text("%.1f FPS - %.3f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
        ImGui::End();
    });
}
bool TestSceneManager::startTest(const std::string& testName) {
    if (mCurrentTest != nullptr) {
        mCurrentTest->end();
        Engine::cleanUp();
    }

    const auto testEntry = mTests.find(testName);
    if (testEntry == mTests.cend()) {
        return false;
    }

    testEntry->second->start();
    return true;
}
}
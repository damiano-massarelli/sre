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

    TestRegisterer::TestRegisterer(std::string name, std::unique_ptr<ITest> test) {
        TestManager::get()->registerTest(name, std::move(test));
    }

    std::unique_ptr<TestManager> TestManager::instance = nullptr;

    TestManager* TestManager::get() {
        if (instance == nullptr) {
            instance = std::make_unique<TestManager>();
        }

        return instance.get();
    }

    void TestManager::registerTest(std::string name, std::unique_ptr<ITest> test) {
        // TODO: check that the test doesn't already exists
        mTests[name] = std::move(test);
    }

    void TestManager::startTests() {
        Engine::uiRenderer.setDebugUIDrawer([this](){
            ImGui::Begin("Tests");
            //ImGui::SetWindowPos(ImVec2::ImVec2());
            //ImGui::SetWindowSize("Tests", ImVec2::ImVec2(150, 500));

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

            ImGui::End();
        });
    }
}
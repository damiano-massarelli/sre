#pragma once

#include "glm/glm.hpp"

#include <map>
#include <memory>
#include <string>


namespace sre {

    /**
     * Test interface.
     */
    class ITestScene {
    public:
        ITestScene() {}
        virtual ~ITestScene() {}

        virtual void start() = 0;
        virtual void end() = 0;
    };

    /**
     * Class used to register a test.
     */
    class TestSceneRegisterer {
    public:
        TestSceneRegisterer(std::string name, std::unique_ptr<ITestScene> test);
    };

    /**
     * Singleton class that contains and run the tests.
     *  Every test register to this manager using the macro definde below.
     *      DECLARE_TEST_SCENE(testName, testClass)
     */
    class TestSceneManager {
    private:
        static std::unique_ptr<TestSceneManager> instance;

        std::map<std::string, std::unique_ptr<ITestScene>> mTests;

        ITestScene* mCurrentTest = nullptr;

    public:
        TestSceneManager() = default;

        static TestSceneManager* get();

        void registerTest(std::string name, std::unique_ptr<ITestScene> test);

        void startTests();
    };
}

#define DECLARE_TEST_SCENE(testName, testClass) \
class testClass : public sre::ITestScene { \
    static sre::TestSceneRegisterer registerer; \
    virtual void start() override; \
    virtual void end() override; \
}; \
sre::TestSceneRegisterer testClass::registerer(testName, std::make_unique<testClass>()); \

#define BEGIN_DECLARE_TEST_SCENE(testClass) \
class testClass : public sre::ITestScene { \
public: \
    testClass() = default; \
private: \
    static sre::TestSceneRegisterer registerer; \
    virtual void start() override; \
    virtual void end() override; \

#define END_DECLARE_TEST_SCENE(testName, testClass) \
}; \
sre::TestSceneRegisterer testClass::registerer(testName, std::make_unique<testClass>()); \

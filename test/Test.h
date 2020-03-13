#pragma once

#include "glm/glm.hpp"

#include <map>
#include <memory>
#include <string>


namespace sre {

    /**
     *  Test interface.
     */
    class ITest {
    public:
        ITest() {}
        virtual ~ITest() {}

        virtual void start() = 0;
        virtual void end() = 0;
    };

    /**
     *  Class used to register a test.
     */
    class TestRegisterer {
    public:
        TestRegisterer(std::string name, std::unique_ptr<ITest> test);
    };

    /**
     *  Singleton class that contains and run the tests.
     *
     *  Every test register to this manager using the macro definde below.
     *      DECLARE_TEST_SCENE(testName, testClass)
     */
    class TestManager {
    private:
        static std::unique_ptr<TestManager> instance;

        std::map<std::string, std::unique_ptr<ITest>> mTests;

        ITest* mCurrentTest = nullptr;

    public:
        TestManager() = default;

        static TestManager* get();

        void registerTest(std::string name, std::unique_ptr<ITest> test);

        void startTests();
    };
}

#define DECLARE_TEST_SCENE(testName, testClass) \
class testClass : public sre::ITest { \
    static sre::TestRegisterer registerer; \
    virtual void start() override; \
    virtual void end() override; \
}; \
sre::TestRegisterer testClass::registerer(testName, std::make_unique<testClass>()); \

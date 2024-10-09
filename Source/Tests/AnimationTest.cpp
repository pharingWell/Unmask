#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(AnimationTest, "Unmask.Tests.AnimationTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)



bool AnimationTest::RunTest(const FString& Parameters)
{
	// Make the test pass by returning true, or fail by returning false.
	return true;
}

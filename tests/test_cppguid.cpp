#include <cppguid/guid.h>
#include <iostream>
using std::string;
using std::cout;

static void assert_expr(bool result, const char* expr, const char* message)
{
    if (result)
        fprintf(stdout, " [PASSED] %s\n", message);
    else
        fprintf(stderr, " [ !!! FAILED !!! ]  (%s)  %s\n", expr, message);
}

#define Assert(expr, message) assert_expr(expr, #expr, message)

int main(int argc, char** argv)
{
    cout << "\n==== CppGuid Tests ====\n";
    using namespace cppguid;

    Guid guid = Guid::create();
    cout << "New GUID: " << guid << "\n";
    Assert(guid.valid(), "New GUID must be valid");
    Assert(!guid.str().empty(), "GUID to string must not be empty");
    Assert(guid.str().size() == 36, "GUID string length must be 36");

    Guid empty;
    Assert(!empty.valid(), "An empty GUID must not be valid");
    Assert(guid != empty, "Created GUID cannot be empty");


    Guid parsed{ guid.str() };
    Assert(parsed == guid, "Parsed GUID must equal original guid");

    string parsedStr = parsed.str();
    string guidStr = guid.str();
    Assert(parsed.str() == guid.str(), "Parsed GUID string must equal original guid string");
    Assert(!Guid::create_str().empty(), "New GUID string cannot be empty");
    Assert(Guid::create_str().size() == 36, "New GUID string length must be 36");
    Assert(Guid{ guid.bytes } == guid, "GUID initialized from bytes must equal original GUID");

    Guid copy = guid;
    Assert(copy.valid(), "Copy constructed GUID must not be empty");
    Assert(copy == guid, "Copy constructed GUID must be equal to original");

    copy = {};
    copy = guid;
    Assert(copy.valid(), "Copy assigned GUID must not be empty");
    Assert(copy == guid, "Copy assigned GUID must be equal to original");
    return 0;
}
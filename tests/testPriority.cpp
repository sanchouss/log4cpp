#include "log4cpp/Priority.hh"
#include <iostream>
#include <stdexcept>
#include <string>

using namespace log4cpp;

struct PriorityEntry {
    Priority::PriorityLevel levelValue;
    const char* name;
};

PriorityEntry priorities[] = {{Priority::EMERG, "FATAL"},   {Priority::FATAL, "FATAL"},  {Priority::ALERT, "ALERT"},
                              {Priority::CRIT, "CRIT"},     {Priority::ERROR, "ERROR"},  {Priority::WARN, "WARN"},
                              {Priority::NOTICE, "NOTICE"}, {Priority::INFO, "INFO"},    {Priority::DEBUG, "DEBUG"},
                              {Priority::TRACE, "TRACE"},   {Priority::NOTSET, "NOTSET"}};

const int numPriorities = sizeof(priorities) / sizeof(priorities[0]);

bool testGetPriorityName() {
    bool passed = true;
    for (int i = 0; i < numPriorities; ++i) {
        const std::string& name = Priority::getPriorityName(priorities[i].levelValue);
        if (name != priorities[i].name) {
            std::cout << "FAIL: getPriorityName(" << priorities[i].levelValue << ") = " << name << ", but expected "
                      << priorities[i].name << "\n";
            passed = false;
        }
    }
    return passed;
}

bool testGetPriorityValue() {
    bool passed = true;
    for (int i = 0; i < numPriorities; ++i) {
        try {
            Priority::Value val = Priority::getPriorityValue(priorities[i].name);
            if (val != priorities[i].levelValue) {
                std::cout << "FAIL: getPriorityValue(" << priorities[i].name << ") = " << val << ", but expected "
                          << priorities[i].levelValue << "\n";
                passed = false;
            }
        } catch (const std::invalid_argument& e) {
            std::cout << "FAIL: getPriorityValue(" << priorities[i].name << ") threw exception: " << e.what() << "\n";
            passed = false;
        }
    }
    return passed;
}

bool testInvalidPriorityValue() {
    try {
        Priority::getPriorityValue("INVALID_NAME");
        std::cout << "FAIL: getPriorityValue(\"INVALID_NAME\") did not throw\n";
        return false;
    } catch (const std::invalid_argument&) {
        return true;
    }
}

bool testIntermediateValues() {
    bool passed = true;
    for (int i = 0; i < numPriorities - 1; ++i) {
        int low = priorities[i].levelValue;
        int high = priorities[i + 1].levelValue;
        // levelValue might be same for adjacent levels
        if (high - low > 1) {
            int mid = low + (high - low) / 2; // pick a value in between
            const std::string& name = Priority::getPriorityName(mid);
            if (name != priorities[i].name) {
                std::cout << "FAIL: getPriorityName(" << mid << ") = " << name << ", but expected " << priorities[i].name
                          << "\n";
                passed = false;
            }
        }
    }
    return passed;
}

int main(int argc, char** argv) {

    std::cout << "priority debug(700): " << Priority::getPriorityName(700) << std::endl;
    std::cout << "priority debug(700): " << Priority::getPriorityValue("DEBUG") << std::endl;
    std::cout << "priority debug(700): " << Priority::getPriorityValue("700") << std::endl;
    try {
        std::cout << "priority debug(700): " << Priority::getPriorityValue("700arghh") << std::endl;
    } catch (std::invalid_argument& e) {
        std::cout << "caught " << e.what() << std::endl;
    }

    bool allPassed = true;

    allPassed &= testGetPriorityName();
    allPassed &= testGetPriorityValue();
    allPassed &= testInvalidPriorityValue();
    allPassed &= testIntermediateValues();

    if (allPassed) {
        std::cout << "All Priority tests passed.\n";
        return 0;
    } else {
        std::cout << "Some Priority tests failed.\n";
        return 1;
    }
}

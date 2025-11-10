//
// Created by fabia on 05.09.2025.
//

#include "Writer.h"

#include <iostream>
#include <ostream>
std::unordered_map<std::string, bool> Writer::allowedTags = {};

/**
 * Change if a tag is printed
 * @param tag tag that is should be changed. Default tags are "uci", "info", "debugging", "other"
 * @param isAllowed true, if this tag is supposed to be printed
 */
void Writer::changeTag(std::string tag, bool isAllowed) {
    allowedTags[tag] = isAllowed;
}

/**
 * Used to print messages to consol
 * Default tags are "uci", "info", "debugging", "other"
 * @param message message that should be printed
 * @param tag tag of the message, the tag decides if a message will be sent
 * @return true, if the message was printed
 */
bool Writer::print(std::string message, std::string tag) {
    if (allowedTags.find(tag) != allowedTags.end()) {
        if (allowedTags[tag]) {
            std::cout << message << std::endl;
            return true;
        }
    }
    return false;
}

/**
 * Enables the tags "info", "debugging", "other"
 */
void Writer::allowDefault() {
    changeTag("info", true);
    changeTag("debugging", true);
    changeTag("other", true);
    changeTag("uci", true);
}

//
// Created by fabia on 05.09.2025.
//

#ifndef WRITER_H
#define WRITER_H
#include <string>
#include <unordered_map>

class Writer {
private:
    static std::unordered_map<std::string, bool> allowedTags;
public:
    static void changeTag(std::string tag, bool isAllowed);
    static bool print(std::string message, std::string tag);
    static void allowDefault();
};




#endif //WRITER_H

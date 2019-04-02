#ifndef CGIHANDLER_H
#define CGIHANDLER_H

#include "common_includes.h"

class CgiHandler
{
public:
    static HashSet<String> cgiMap;

    static void loadCgiPrograms(const char* file=CGIPROGRAMS_MAPFILE) {
        std::fstream ifs(file, ios::in);
        if (not ifs.is_open())
            return;

         if (cgiMap.size())
            cgiMap.clear();

        String programName;
        while (not ifs.eof()) {
            ifs >> programName;
        }
    }

public:
    static bool isCgi(const String& name);

    static String getCgiProgrmaName(const String& str);

    static String processCgi(const String& cgiName, const String& inputArgs, char sep=' ');
};


#endif // CGIHANDLER_H

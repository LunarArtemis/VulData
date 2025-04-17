#include "std_testcase.h"

#include <wchar.h>

#define SOURCE_STRING L"abc/opqrstu"



void func1()
{
    int j;
    for(j = 0; j < 1; j++)
    {
        {
            wchar_t string1[] = SOURCE_STRING;
            wchar_t string2[] = SOURCE_STRING;
            wchar_t * slashInString1;
            size_t indexOfSlashInString1;
            slashInString1 = wcschr(string1, L'/');
            if (slashInString1 == NULL)
            {
                exit(1);
            }
            indexOfSlashInString1 = (size_t)(slashInString1 - string2);
            printUnsignedLine(indexOfSlashInString1);
        }
    }
}



int main(int argc, char * argv[])
{
    srand( (unsigned)time(NULL) );

    printLine("Calling ...");
    func1();
    printLine("Finished");
    return 0;
}


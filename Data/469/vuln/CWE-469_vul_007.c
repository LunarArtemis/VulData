#include "std_testcase.h"

#include <wchar.h>

#define SOURCE_STRING "abc/opqrstu"

static int staticFive = 5;



void func1()
{
    if(staticFive==5)
    {
        {
            char string1[] = SOURCE_STRING;
            char string2[] = SOURCE_STRING;
            char * slashInString1;
            size_t indexOfSlashInString1;
            slashInString1 = strchr(string1, '/');
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


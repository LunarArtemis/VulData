#include "std_testcase.h"

#include <wchar.h>

#define SOURCE_STRING "abc/opqrstu"

static const int STATIC_CONST_TRUE = 1; 
static const int STATIC_CONST_FALSE = 0; 



static void func2()
{
    if(STATIC_CONST_FALSE)
    {
        printLine("Benign, fixed string");
    }
    else
    {
        {
            char string1[] = SOURCE_STRING;
            char * slashInString1;
            size_t indexOfSlashInString1;
            slashInString1 = strchr(string1, '/');
            if (slashInString1 == NULL)
            {
                exit(1);
            }
            indexOfSlashInString1 = (size_t)(slashInString1 - string1);
            printUnsignedLine(indexOfSlashInString1);
        }
    }
}

static void func3()
{
    if(STATIC_CONST_TRUE)
    {
        {
            char string1[] = SOURCE_STRING;
            char * slashInString1;
            size_t indexOfSlashInString1;
            slashInString1 = strchr(string1, '/');
            if (slashInString1 == NULL)
            {
                exit(1);
            }
            indexOfSlashInString1 = (size_t)(slashInString1 - string1);
            printUnsignedLine(indexOfSlashInString1);
        }
    }
}

void func4()
{
    func2();
    func3();
}



int main(int argc, char * argv[])
{
    srand( (unsigned)time(NULL) );

    printLine("Calling ...");
    func4();
    printLine("Finished");
    return 0;
}


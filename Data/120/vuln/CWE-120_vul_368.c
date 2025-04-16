int extractVersionQuadFromString(const char* string, int* quad) {
    char versionString[128];
    char* nextDot;
    char* nextNumber;
    int i;
    
    strcpy(versionString, string);
    nextNumber = versionString;
    
    for (i = 0; i < 4; i++) {
        if (i == 3) {
            nextDot = strchr(nextNumber, '\0');
        }
        else {
            nextDot = strchr(nextNumber, '.');
        }
        if (nextDot == NULL) {
            return -1;
        }
        
        // Cut the string off at the next dot
        *nextDot = '\0';
        
        quad[i] = atoi(nextNumber);
        
        // Move on to the next segment
        nextNumber = nextDot + 1;
    }
    
    return 0;
}
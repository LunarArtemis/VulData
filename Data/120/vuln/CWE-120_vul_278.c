void EXPORT_CALL
lou_logFile(const char *fileName) {
	if (logFile) {
		fclose(logFile);
		logFile = NULL;
	}
	if (fileName == NULL || fileName[0] == 0) return;
	if (initialLogFileName[0] == 0) strcpy(initialLogFileName, fileName);
	logFile = fopen(fileName, "a");
	if (logFile == NULL && initialLogFileName[0] != 0)
		logFile = fopen(initialLogFileName, "a");
	if (logFile == NULL) {
		fprintf(stderr, "Cannot open log file %s\n", fileName);
		logFile = stderr;
	}
}
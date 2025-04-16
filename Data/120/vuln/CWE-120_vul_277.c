char *EXPORT_CALL
lou_setDataPath(const char *path) {
	static char dataPath[MAXSTRING];
	dataPathPtr = NULL;
	if (path == NULL) return NULL;
	strcpy(dataPath, path);
	dataPathPtr = dataPath;
	return dataPathPtr;
}
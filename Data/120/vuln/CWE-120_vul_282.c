static char *
resolveSubtable(const char *table, const char *base, const char *searchPath) {
	char *tableFile;
	static struct stat info;

	if (table == NULL || table[0] == '\0' ||
			strlen(table) >= MAXSTRING * sizeof(char) * 2)
		return NULL;
	tableFile = (char *)malloc(MAXSTRING * sizeof(char) * 2);

	//
	// First try to resolve against base
	//
	if (base) {
		int k;
		if (strlen(base) >= MAXSTRING * sizeof(char) * 2) goto failure;
		strcpy(tableFile, base);
		k = (int)strlen(tableFile);
		while (k >= 0 && tableFile[k] != '/' && tableFile[k] != '\\') k--;
		tableFile[++k] = '\0';
		if (strlen(tableFile) + strlen(table) >= MAXSTRING * sizeof(char) * 2)
			goto failure;
		strcat(tableFile, table);
		if (stat(tableFile, &info) == 0 && !(info.st_mode & S_IFDIR)) {
			_lou_logMessage(LOU_LOG_DEBUG, "found table %s", tableFile);
			return tableFile;
		}
	}

	//
	// It could be an absolute path, or a path relative to the current working
	// directory
	//
	strcpy(tableFile, table);
	if (stat(tableFile, &info) == 0 && !(info.st_mode & S_IFDIR)) {
		_lou_logMessage(LOU_LOG_DEBUG, "found table %s", tableFile);
		return tableFile;
	}

	//
	// Then search `LOUIS_TABLEPATH`, `dataPath` and `programPath`
	//
	if (searchPath[0] != '\0') {
		char *dir;
		int last;
		char *cp;
		char *searchPath_copy = strdup(searchPath);
		for (dir = searchPath_copy;; dir = cp + 1) {
			for (cp = dir; *cp != '\0' && *cp != ','; cp++)
				;
			last = (*cp == '\0');
			*cp = '\0';
			if (dir == cp) dir = ".";
			if (strlen(dir) + strlen(table) + 1 >= MAXSTRING * sizeof(char) * 2) {
				free(searchPath_copy);
				goto failure;
			}
			sprintf(tableFile, "%s%c%s", dir, DIR_SEP, table);
			if (stat(tableFile, &info) == 0 && !(info.st_mode & S_IFDIR)) {
				_lou_logMessage(LOU_LOG_DEBUG, "found table %s", tableFile);
				free(searchPath_copy);
				return tableFile;
			}
			if (last) break;
			if (strlen(dir) + strlen(table) + 16 >= MAXSTRING * sizeof(char) * 2) {
				free(searchPath_copy);
				goto failure;
			}
			sprintf(tableFile, "%s%c%s%c%s%c%s", dir, DIR_SEP, "liblouis", DIR_SEP,
					"tables", DIR_SEP, table);
			if (stat(tableFile, &info) == 0 && !(info.st_mode & S_IFDIR)) {
				_lou_logMessage(LOU_LOG_DEBUG, "found table %s", tableFile);
				free(searchPath_copy);
				return tableFile;
			}
			if (last) break;
		}
		free(searchPath_copy);
	}
failure:
	free(tableFile);
	return NULL;
}
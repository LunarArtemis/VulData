char *                                  // Proxy string or NULL if failed.
pacparser_find_proxy(const char *url, const char *host)
{
  char *error_prefix = "pacparser.c: pacparser_find_proxy:";
  if (_debug()) print_error("DEBUG: Finding proxy for URL: %s and Host:"
                        " %s\n", url, host);
  jsval rval;
  char *script;
  if (url == NULL || (strcmp(url, "") == 0)) {
    print_error("%s %s\n", error_prefix, "URL not defined");
    return NULL;
  }
  if (host == NULL || (strcmp(host,"") == 0)) {
    print_error("%s %s\n", error_prefix, "Host not defined");
    return NULL;
  }
  if (cx == NULL || global == NULL) {
    print_error("%s %s\n", error_prefix, "Pac parser is not initialized.");
    return NULL;
  }
  // Test if findProxyForURL is defined.
  script = "typeof(findProxyForURL);";
  if (_debug()) print_error("DEBUG: Executing JavaScript: %s\n", script);
  JS_EvaluateScript(cx, global, script, strlen(script), NULL, 1, &rval);
  if (strcmp("function", JS_GetStringBytes(JS_ValueToString(cx, rval))) != 0) {
    print_error("%s %s\n", error_prefix,
		  "Javascript function findProxyForURL not defined.");
    return NULL;
  }

  // URL-encode "'" as we use single quotes to stick the URL into a temporary script.
  char *sanitized_url = str_replace(url, "'", "%27");
  // Hostname shouldn't have single quotes in them
  if (strchr(host, '\'')) {
    print_error("%s %s\n", error_prefix,
		"Invalid hostname: hostname can't have single quotes.");
    return NULL;
  }

  script = (char*) malloc(32 + strlen(url) + strlen(host));
  script[0] = '\0';
  strcat(script, "findProxyForURL('");
  strcat(script, sanitized_url);
  strcat(script, "', '");
  strcat(script, host);
  strcat(script, "')");
  if (_debug()) print_error("DEBUG: Executing JavaScript: %s\n", script);
  if (!JS_EvaluateScript(cx, global, script, strlen(script), NULL, 1, &rval)) {
    print_error("%s %s\n", error_prefix, "Problem in executing findProxyForURL.");
    free(sanitized_url);
    free(script);
    return NULL;
  }
  free(sanitized_url);
  free(script);
  return JS_GetStringBytes(JS_ValueToString(cx, rval));
}
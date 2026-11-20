#ifndef LOG_MESSAGE_H_
#define LOG_MESSAGE_H_

#if defined(LOG_MESSAGE_IMPLEMENTATION)
    void logMessageImpl(LogLevel level, const char *file, int line, const char *format, ...)
    {
	FILE *stream = NULL;

	if (level < LOG_INFO || level > LOG_ERROR) {
	    fprintf(stderr, "[ERROR] %s:%d: Invalid log level\n", file, line);
	    fflush(stderr);
	    return; 
	}
	stream = (level == LOG_ERROR) ? stderr : stdout;

	const char *levelName[] = {
	    [LOG_INFO] = "INFO",
	    [LOG_WARNING] = "WARNING",
	    [LOG_ERROR] = "ERROR"
	};

	if (stream == stderr)
	    fprintf(stream, "[%s] %s:%d: ", levelName[level], file, line);
	else 
	    fprintf(stream, "[%s] ", levelName[level]);
	
	va_list args;
	va_start(args, format);
	vfprintf(stream, format, args);
	va_end(args);
	fflush(stream);
    }
#endif
#endif

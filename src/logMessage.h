#ifndef LOG_MESSAGE_H_
#define LOG_MESSAGE_H_

#if defined(LOG_MESSAGE_IMPLEMENTATION)
    void logMessageImpl(LogLevel level, const char *file, int line, const char *format, ...)
    {
	FILE *stream = NULL;
	if (level == LOG_NONE) return;

	stream = (level == LOG_ERROR) ? stderr : stdout;
	if (level < LOG_INFO || level > LOG_ERROR)
	    return;

	const char *levelName[] = {
	    [LOG_INFO] = "INFO",
	    [LOG_WARNING] = "WARNING",
	    [LOG_ERROR] = "ERROR"
	};

	if (stream == stderr)
	    fprintf(stream, "%s:%d: [%s] ", file, line, levelName[level]);
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

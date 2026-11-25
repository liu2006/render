#ifndef M_LOG_MESSAGE_H_
#define M_LOG_MESSAGE_H_

#if defined(LOG_MESSAGE_IMPLEMENTATION)
    void logMessageImpl(LogLevel level, const char *file, int line, const char *format, ...)
    {
	FILE *stream = NULL;

	if (level < LOG_INFO || level > LOG_ERROR) {
	    fprintf(stderr, "\x1b[31m[ERROR] %s:%d: \x1b[0mInvalid log level\n", file, line);
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
	    fprintf(stream, "\x1b[31m[%s] %s:%d: \x1b[0m", levelName[level], file, line);
	else 
	    fprintf(stream, "\x1b[32m[%s] \x1b[0m", levelName[level]);
	
	va_list args;
	va_start(args, format);
	vfprintf(stream, format, args);
	va_end(args);
	fflush(stream);
    }
#endif
#endif

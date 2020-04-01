#include "targum_err.h"

#ifdef OS_WINDOWS
#	define TARGUM_LIB
#endif

#define COLOR_RED       "\x1B[31m"
#define COLOR_MAGENTA   "\x1B[35m"
#define COLOR_RESET     "\033[0m"

static struct {
	size_t errs;
} g_err_state;


TARGUM_API void targum_err(const char filename[restrict], const char errtype[restrict], const size_t line, const size_t col, const char err[restrict], ...)
{
	va_list args;
	va_start(args, err);
	
	fprintf(stderr, "(%s:%zu:%zu) %s%s%s: **** ", filename, line, col, COLOR_RED, errtype, COLOR_RESET);
	vfprintf(stderr, err, args);
	fprintf(stderr, " ****\n");
	va_end(args);
	
	g_err_state.errs++;
}

TARGUM_API void targum_warn(const char filename[restrict], const char warntype[restrict], const size_t line, const size_t col, const char warn[restrict], ...)
{
	va_list args;
	va_start(args, warn);
	
	fprintf(stderr, "(%s:%zu:%zu) %s%s%s: **** ", filename, line, col, COLOR_MAGENTA, warntype, COLOR_RESET);
	vfprintf(stderr, warn, args);
	fprintf(stderr, " ****\n");
	va_end(args);
}

TARGUM_API void targum_msg(const char custom[restrict], ...)
{
	va_list args;
	va_start(args, custom);
	vprintf(custom, args);
	va_end(args);
}

TARGUM_API size_t targum_error_count(void)
{
	return g_err_state.errs;
}

TARGUM_API bool targum_is_fataled(void)
{
	return g_err_state.errs > 0;
}

TARGUM_API void targum_err_reset(void)
{
	g_err_state.errs = 0;
}

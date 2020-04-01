#ifndef TARGUM_ERRSYS_INCLUDED
#	define TARGUM_ERRSYS_INCLUDED

#ifdef TARGUM_DLL
#	ifndef TARGUM_LIB 
#		define TARGUM_API __declspec(dllimport)
#	else
#		define TARGUM_API __declspec(dllexport)
#	endif
#else
#	define TARGUM_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
#include "harbol_common_defines.h"
#include "harbol_common_includes.h"


TARGUM_API NEVER_NULL(5) void targum_err(const char filename[], const char errtype[], const size_t line, const size_t col, const char err[], ...);

TARGUM_API NEVER_NULL(5) void targum_warn(const char filename[], const char warntype[], const size_t line, const size_t col, const char warn[], ...);

TARGUM_API NO_NULL void targum_msg(const char custom[], ...);
TARGUM_API size_t targum_error_count(void);
TARGUM_API bool targum_is_fataled(void);
TARGUM_API void targum_err_reset(void);


#ifdef __cplusplus
}
#endif

#endif /** TARGUM_ERRSYS_INCLUDED */

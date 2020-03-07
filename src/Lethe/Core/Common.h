#pragma once

#if !defined(LETHE_DEBUG) && (defined(DEBUG) || defined(_DEBUG))
#	define LETHE_DEBUG 1
#endif

// thanks Qt
#if defined(QT_NO_DEBUG) && !defined(NDEBUG)
#   define NDEBUG 1
#endif

#if !LETHE_DEBUG && !defined(NDEBUG)
#	define NDEBUG 1
#endif

// helper macros to simplify checks where a bool call fails
#define LETHE_RET_FALSE(x) do { if (!(x)) return 0; } while(0)
#define LETHE_RET_TRUE(x) do { if ((x)) return 1; } while(0)
#define LETHE_LOG_FALSE(x) do { if (!(x)) lethe::Log("`%s` failed at line %d in %s", #x, __LINE__, __FILE__); } while(0)
#define LETHE_RET_FALSE_MSG(x, msg) do { if (!(x)) { lethe::Log(msg); return 0; } } while(0)

// allow the possibility of building a dynamic library
#if defined(_WIN32) && defined(LETHE_DYNAMIC)
#	if defined(LETHE_BUILD)
#		define LETHE_API __declspec(dllexport)
#	else
#		define LETHE_API __declspec(dllimport)
#	endif
#else
#	define LETHE_API
#endif

#if _MSC_VER
#	pragma warning(disable:4251)	// disable need dll-interface for templates
#	pragma warning(disable:4127)	// disable silly conditional expression is constant warning
#endif

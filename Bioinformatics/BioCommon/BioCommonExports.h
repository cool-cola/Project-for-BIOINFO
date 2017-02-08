#ifdef BIO_COMMON_DLL_EXPORTS
#define BIO_COMMON_DLL_EXPORT __declspec(dllexport)
#else
#define BIO_COMMON_DLL_EXPORT __declspec(dllimport)
#endif // BIO_RANDOM_DLL_EXPORTS
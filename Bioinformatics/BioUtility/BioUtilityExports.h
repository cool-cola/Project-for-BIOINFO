#ifdef BIO_UTILITY_DLL_EXPORTS
#define BIO_UTILITY_DLL_EXPORT __declspec(dllexport)
#else
#define BIO_UTILITY_DLL_EXPORT __declspec(dllimport)
#endif // BIO_UTILITY_DLL_EXPORTS
    
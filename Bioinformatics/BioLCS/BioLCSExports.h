#ifdef BIO_LCS_DLL_EXPORTS
#define BIO_LCS_DLL_EXPORT __declspec(dllexport)
#else
#define  BIO_LCS_DLL_EXPORT __declspec(dllimport)
#endif //BIO_LCS_DLL_EXPORTS
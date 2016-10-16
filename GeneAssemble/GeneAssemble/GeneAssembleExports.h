#ifdef GENE_ASSEMBLE_DLL_EXPORTS
#define GENE_ASSEMBLE_DLL_EXPORT __declspec(dllexport)
#else
#define GENE_ASSEMBLE_DLL_EXPORT __declspec(dllimport)
#endif // GENE_ASSEMBLE_DLL_EXPORTS
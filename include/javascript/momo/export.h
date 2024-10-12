#pragma once

#ifdef _MSC_VER
#define MOMO_DLL_EXPORT __declspec(noinline dllexport)
#else
#define MOMO_DLL_EXPORT __attribute__((visibility("default"), noinline, used))
#endif

#ifdef __cplusplus
#define MOMO_EXTERN_C extern "C"
#else
#define MOMO_EXTERN_C
#endif

#define MOMO_DLL_EXPORT_C MOMO_EXTERN_C MOMO_DLL_EXPORT

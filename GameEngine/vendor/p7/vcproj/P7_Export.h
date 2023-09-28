
#ifndef P7_EXPORT_H
#define P7_EXPORT_H

#ifdef P7_STATIC_DEFINE
#  define P7_EXPORT
#  define P7_NO_EXPORT
#else
#  ifndef P7_EXPORT
#    ifdef P7_EXPORTS
        /* We are building this library */
#      define P7_EXPORT 
#    else
        /* We are using this library */
#      define P7_EXPORT 
#    endif
#  endif

#  ifndef P7_NO_EXPORT
#    define P7_NO_EXPORT 
#  endif
#endif

#ifndef P7_DEPRECATED
#  define P7_DEPRECATED __declspec(deprecated)
#endif

#ifndef P7_DEPRECATED_EXPORT
#  define P7_DEPRECATED_EXPORT P7_EXPORT P7_DEPRECATED
#endif

#ifndef P7_DEPRECATED_NO_EXPORT
#  define P7_DEPRECATED_NO_EXPORT P7_NO_EXPORT P7_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef P7_NO_DEPRECATED
#    define P7_NO_DEPRECATED
#  endif
#endif

#endif /* P7_EXPORT_H */

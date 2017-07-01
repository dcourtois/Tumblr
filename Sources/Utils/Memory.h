#ifndef __MEMORY_H__
#define	__MEMORY_H__

#if defined(NO_PCH)
#	include <cstdio>
#endif

//!
//! @file Memory.h
//!
//! This file contains helper macros used to track allocated memory.
//!

#if defined(NEW)
#	undef NEW
#endif
#if defined(NEW_ARRAY)
#	undef NEW_ARRAY
#endif
#if defined(DELETE)
#	undef DELETE
#endif
#if defined(DELETE_ARRAY)
#	undef DELETE_ARRAY
#endif

#if !defined(QT_NO_DEBUG)

extern void * operator new(std::size_t size, const char * filename, int line);
extern void * operator new [] (std::size_t size, const char * filename, int line);
extern void operator delete(void * pointer) noexcept;
extern void operator delete(void * pointer, const char * filename, int line);
extern void operator delete [] (void * pointer) noexcept;
extern void operator delete [] (void * pointer, const char * filename, int line);

#	define	NEW				new(__FILE__, __LINE__)
#	define	NEW_ARRAY		new [](__FILE__, __LINE__)
#	define	DELETE			delete
#	define	DELETE_ARRAY	delete []

#else

#	define	NEW				new
#	define	NEW_ARRAY		new []
#	define	DELETE			delete
#	define	DELETE_ARRAY	delete []

#endif


#endif // __MEMORY_H__

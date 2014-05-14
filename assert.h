/* 
 * File:   assert.h
 * Author: Nathaniel Abalos
 *
 * Created on April 9, 2013, 11:46 PM
 */

#ifndef ASSERT_H
#define	ASSERT_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef __DEBUG 
/*
 * For PIC24F repeat the halt instruction 2 times to ensure it stops,
 * avoiding the idiotic lame "breakpoint skidding" issue...
 */
#define HALT_TO_DEBUGGER(_why) {__asm__ volatile (".pword 0xDA4000");__asm__ volatile (".pword 0xDA4000");}
#else
#define HALT_TO_DEBUGGER(_why) {}
#endif

#if defined(NDEBUG) || !defined(__DEBUG)
#define assert(_x) {} /* no code for checking assertions in this build */
#else
#if !defined(HALT_TO_DEBUGGER)
#error HALT_TO_DEBUGGER(_why) must be defined, for PIC24F {__asm__ volatile (".pword 0xDA4000");__asm__ volatile (".pword 0xDA4000");} in hardwareProfile.h
#endif
/*
 *  replaces: ((void)((expr) ? 0 : (_fassert (__LINE__, __FILE__, #expr, __ASSERT_FUNC), 0)))
 */
#define assert(_x) {if( ! (_x)) {HALT_TO_DEBUGGER("Assertion Failure");}}
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* ASSERT_H */


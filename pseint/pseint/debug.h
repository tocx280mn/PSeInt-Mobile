#ifndef DEBUG_H
#define DEBUG_H

// breakpoint en codigo
#ifdef _DEBUG
#	define _revienta(cond) { if (cond) asm("int3"); asm("nop"); }
#	define _if_debug(x) x
#else // release (no genera codigo en el binario)
#	define _revienta(cond) ((void)0)
#	define _if_debug(x)
#endif

#define _warning(cond) _revienta(cond)
#define _impossible _revienta(true)
#define _expects(cond) _revienta(!(cond))
#define _ensures(cond) _revienta(!(cond))

#endif

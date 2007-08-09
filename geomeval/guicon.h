/*  GUICON.H

    Creates a WIN32 console window and redirects stdio, stderr,
	cout, and cerr to it.

	Needed for WIN32 build; do not compile for UNIX build 
	
	Code from Windows Developer Journal, December 1997
	http://www.halcyon.com/~ast/dload/guicon.htm
*/

#ifndef __GUICON_H__

#define __GUICON_H__


void RedirectIOToConsole();


#endif

/* End of File */

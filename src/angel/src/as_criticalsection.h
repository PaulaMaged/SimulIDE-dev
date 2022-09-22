/*
   AngelCode Scripting Library
   Copyright (c) 2003-2017 Andreas Jonsson

   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any
   damages arising from the use of this software.

   Permission is granted to anyone to use this software for any
   purpose, including commercial applications, and to alter it and
   redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product
      documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and
      must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
      distribution.

   The original version of this library can be located at:
   http://www.angelcode.com/angelscript/

   Andreas Jonsson
   andreas@angelcode.com
*/



//
// as_criticalsection.h
//
// Classes for multi threading support
//

#ifndef AS_CRITICALSECTION_H
#define AS_CRITICALSECTION_H

#include "as_config.h"

BEGIN_AS_NAMESPACE

#define DECLARECRITICALSECTION(x)
#define ENTERCRITICALSECTION(x)
#define LEAVECRITICALSECTION(x)

inline bool tryEnter() { return true; }
#define TRYENTERCRITICALSECTION(x) tryEnter()

#define DECLAREREADWRITELOCK(x)
#define ACQUIREEXCLUSIVE(x)
#define RELEASEEXCLUSIVE(x)
#define ACQUIRESHARED(x)
#define RELEASESHARED(x)

END_AS_NAMESPACE

#endif


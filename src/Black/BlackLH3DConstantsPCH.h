#ifndef BW1_DECOMP_BLACK_LH3D_CONSTANTS_PCH_INCLUDED_H
#define BW1_DECOMP_BLACK_LH3D_CONSTANTS_PCH_INCLUDED_H

// BlackPCH.h with CellSize and LH3DLib's EighthScale and PI_OVER_2 defined ahead of it. The units built with it hold the two
// statics as only a precompiled header's can be held: initialised first and in this order, yet laid out in .bss
// where the unit first uses them (docs/msvc6_pch.md). The original's one precompiled header held them; until
// BlackPCH.h can, the units whose targets show it use this one.
#include "CellSize.h"

#include <Lionhead/LH3DLib/development/LH3DScaleConstants.h>
#include <Lionhead/LH3DLib/development/LH3DMathConstants.h>

#include "BlackPCH.h"

#endif /* BW1_DECOMP_BLACK_LH3D_CONSTANTS_PCH_INCLUDED_H */

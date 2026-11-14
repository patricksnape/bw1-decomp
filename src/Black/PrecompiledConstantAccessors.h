#ifndef BW1_DECOMP_PRECOMPILED_CONSTANT_ACCESSORS_INCLUDED_H
#define BW1_DECOMP_PRECOMPILED_CONSTANT_ACCESSORS_INCLUDED_H

#include <Lionhead/LH3DLib/development/LH3DMathConstants.h>  /* For PI_OVER_2 */
#include <Lionhead/LH3DLib/development/LH3DScaleConstants.h> /* For EighthScale */

#include "CellSize.h" /* For CellSize */

// fabricated: these stand in for the unknown uncalled header inlines that use, in this order, the constants
// BlackLH3DConstantsPCH.h defines. A unit holds a precompiled header's constant only if it uses it.
inline float GetPrecompiledCellSize()
{
	return CellSize;
}

inline float GetPrecompiledHalfPi()
{
	return PI_OVER_2;
}

inline float GetPrecompiledEighthScale()
{
	return EighthScale;
}

#endif /* BW1_DECOMP_PRECOMPILED_CONSTANT_ACCESSORS_INCLUDED_H */

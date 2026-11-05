#ifndef BW1_DECOMP_MPFE_RESET_CLAN_INCLUDED_H
#define BW1_DECOMP_MPFE_RESET_CLAN_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <wchar.h>  /* For wchar_t */

#include "MPFEMessageObject.h" /* For struct MPFEMessageObject */

// Forward Declares

struct MPFEPlayerDetails;

class MPFEResetClan : public MPFEMessageObject
{
public:
	// Static methods

	// BW1W120 0062c460 BW1M119 013a7d40
	static void ProcessIncoming(wchar_t* message, MPFEPlayerDetails* from);

	// Constructors

	// BW1W120 0062c360 BW1M119 013a7e40
	MPFEResetClan(unsigned long team);
};

#endif /* BW1_DECOMP_MPFE_RESET_CLAN_INCLUDED_H */

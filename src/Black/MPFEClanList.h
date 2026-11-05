#ifndef BW1_DECOMP_MPFE_CLAN_LIST_INCLUDED_H
#define BW1_DECOMP_MPFE_CLAN_LIST_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <wchar.h>  /* For wchar_t */

#include "MPFEMessageObject.h" /* For struct MPFEMessageObject */

// Forward Declares

struct MPFEPlayerDetails;

class MPFEClanList : public MPFEMessageObject
{
public:
	// Static methods

	// BW1W120 0062bf60 BW1M119 013a8240
	static void ProcessIncoming(wchar_t* message, MPFEPlayerDetails* from);

	// Constructors

	// BW1W120 0062bba0 BW1M119 013a84a0
	MPFEClanList();
};

#endif /* BW1_DECOMP_MPFE_CLAN_LIST_INCLUDED_H */

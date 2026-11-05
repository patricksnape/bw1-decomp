#ifndef BW1_DECOMP_MPFE_CONNECTION_TYPE_INCLUDED_H
#define BW1_DECOMP_MPFE_CONNECTION_TYPE_INCLUDED_H

#include <assert.h> /* For static_assert */
#include <stdint.h> /* For uint8_t */

#include <Lionhead/LHLib/ver5.0/LHLinkedList.h> /* For LHLinkedList */

#include <re_common.h> /* For bool32_t */

enum MPFE_CONNECTION_TYPE
{
	MPFE_CONNECTION_TYPE_DIALUP = 0,
	MPFE_CONNECTION_TYPE_LAN = 1,
};

// Status codes Dialup::Connect passes to its callback.
enum DIALUP_STATUS
{
	DIALUP_STATUS_AUTHENTICATION_ERROR = -3,
	DIALUP_STATUS_ERROR = -2,
	DIALUP_STATUS_DISCONNECTED = -1,
	DIALUP_STATUS_CONNECTING = 1,
	DIALUP_STATUS_CONNECTED = 2,
	DIALUP_STATUS_AUTHENTICATING = 3,
	DIALUP_STATUS_AUTHENTICATED = 4,
	DIALUP_STATUS_LOGGING_ON = 5,
	DIALUP_STATUS_PASSWORD_EXPIRED = 6,
};

#define MPFE_CONNECTION_TYPE_NAME_LENGTH    0x80
#define MPFE_CONNECTION_TYPE_ADDRESS_LENGTH 0x100

class MPFEConnectionType
{
public:
	// BW1W120 00d40e28
	static LHLinkedList<MPFEConnectionType*> ConnectionList;

	MPFE_CONNECTION_TYPE Type;                                         /* 0x0 */
	uint8_t              field_0x4[0x83c];                             /* 0x4 */
	wchar_t              Name[MPFE_CONNECTION_TYPE_NAME_LENGTH];       /* 0x840 */
	int                  EntryIndex;                                   /* 0x940 */
	bool                 Connected;                                    /* 0x944 */
	bool                 field_0x945;                                  /* 0x945 */
	bool                 IsDefault;                                    /* 0x946 */
	bool                 field_0x947;                                  /* 0x947 */
	char                 Address[MPFE_CONNECTION_TYPE_ADDRESS_LENGTH]; /* 0x948 */

	// Static methods

	// BW1W120 0062d1c0 BW1M119 013a9fd0
	static void ReleaseDialup();
	// BW1W120 0062d1e0 BW1M119 013a9f90
	static bool DialupPresent();
	// BW1W120 0062d210 BW1M119 013a9d50
	static void InitialiseConnectionList();
	// BW1W120 0062d6e0 BW1M119 013a9b60
	static MPFEConnectionType* ModemConnected();

	// Non-virtual methods

	// BW1W120 0062d710 BW1M119 013a9b20
	bool32_t DialModem(wchar_t* status_text);
	// BW1W120 0062d740 BW1M119 013a9ae0
	void HangupModem();
};

static_assert(sizeof(MPFEConnectionType) == 0xa48, "MPFEConnectionType size is incorrect");

#endif /* BW1_DECOMP_MPFE_CONNECTION_TYPE_INCLUDED_H */

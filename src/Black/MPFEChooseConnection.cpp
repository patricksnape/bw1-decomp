#include "MPFEChooseConnection.h"

#include "ColourConstants.h"   /* For White */
#include "GameTimeConstants.h" /* For SecondsPerYear */

#include <string.h>  /* For wcscpy */
#include <windows.h> /* For timeGetTime */

#include <Lionhead/LHLib/ver5.0/LHLinkedListIterator.h> /* For LHLinkedListIterator */
#include <Lionhead/LHLib/ver5.0/LHWin.h>                /* For operator new(size_t, const char*, uint32_t) */
#include <Lionhead/LHLog/ver4.0/LHRegistry.h>           /* For RegistryRetrieveULong */
#include <Lionhead/LHLog/ver4.0/LHSPrintf.h>            /* For LHSPrintfW */
#include <Lionhead/LHMultiplayer/ver4.0/LHLobby.h>      /* For LHLobby::SendFullChecksum */

#include <chlasm/HelpTextEnums.h>

#include "alexmfc.h"              /* For SetupBigButton, SetupEdit, SetupList, SetupStaticText */
#include "HelpText.h"             /* For HelpTextDataBase::HelpTextDatabase */
#include "IpSpecialDialog.h"      /* For TimIPSpecial */
#include "MPFEConnectionStatus.h" /* For MPFEConnectionStatus::Status */
#include "MPFEConnectionType.h"   /* For MPFEConnectionType */

#if defined(VERSION_BW1W100) || defined(VERSION_BW1WCI)
#define MPFE_CHOOSE_CONNECTION_FILE "C:\\dev\\black\\MPFEChooseConnection.cpp"
#elif defined(VERSION_BW1W110)
#define MPFE_CHOOSE_CONNECTION_FILE "C:\\dev\\Black\\MPFEChooseConnection.cpp"
#elif defined(VERSION_BW1W130)
#define MPFE_CHOOSE_CONNECTION_FILE "D:\\Work\\Games\\dd\\Black\\MPFEChooseConnection.cpp"
#else
#define MPFE_CHOOSE_CONNECTION_FILE "C:\\dev\\MP\\Black\\MPFEChooseConnection.cpp"
#endif

#ifdef VERSION_BW1WCI
#define CREATURE_ISLE_DIALOG_HELP_TEXTS 0
#define CREATURE_ISLE_MPFE_HELP_TEXTS   0
#define CREATURE_ISLE_HELP_TEXTS        0
#else
#define CREATURE_ISLE_DIALOG_HELP_TEXTS 8
#define CREATURE_ISLE_MPFE_HELP_TEXTS   25
#define CREATURE_ISLE_HELP_TEXTS        18
#endif

#define MPFE_CHOOSE_CONNECTION_COLOUR       0xe0e0e0
#define MPFE_CHOOSE_CONNECTION_REGISTRY_KEY "Software\\Lionhead Studios Ltd\\Black & White\\BWSetup"

// BW1W120 0062b200 BW1M119 013a7a80
static void OnClickBack(SetupBox* box, SetupControl* control);
// BW1W120 0062b210 BW1M119 013a7910
static void OnClickContinue(SetupBox* box, SetupControl* control);
// BW1W120 0062b310 BW1M119 013a7760
static void OnClickConnectionBox(SetupBox* box, SetupControl* control);
// BW1W120 0062b830 BW1M119 013a7810
static void OnMessageBox(SetupBox* box, SetupControl* control, int button);

MPFEChooseConnection MPFEChooseConnection::Instance;

MPFEChooseConnection::MPFEChooseConnection()
{
	BackButton = NULL;
	ContinueButton = NULL;
	ConnectionList = NULL;
	Connected = true;
	HiddenEdit = NULL;
	TimeoutStart = -1;
	DialBox = NULL;
	Init(780, 500, ControlCallback);
	setup_box->TallBackground = 1;
}

MPFEChooseConnection::~MPFEChooseConnection()
{
	delete BackButton;
	BackButton = NULL;
	delete ContinueButton;
	ContinueButton = NULL;
	delete TitleText;
	TitleText = NULL;
	delete ConnectionList;
	ConnectionList = NULL;
	delete HiddenEdit;
	HiddenEdit = NULL;
	MPFEConnectionType::ReleaseDialup();
}

void MPFEChooseConnection::SetTimeout(uint32_t timeout)
{
	TimeoutStart = timeGetTime();
	Timeout = timeout;
}

void __stdcall MPFEChooseConnection::DialCallback(int status, void* data)
{
	Instance.OnDialStatus(status, (wchar_t*)data);
}

void __stdcall MPFEChooseConnection::OnDialStatus(int status, wchar_t* text)
{
	switch (status)
	{
	case DIALUP_STATUS_CONNECTING:
		wcscpy(text, HelpTextDataBase::HelpTextDatabase.GetHelpText(HELP_TEXT_DIALOG_CONNECTING -
		                                                            CREATURE_ISLE_DIALOG_HELP_TEXTS));
		break;
	case DIALUP_STATUS_AUTHENTICATING:
		wcscpy(text, HelpTextDataBase::HelpTextDatabase.GetHelpText(HELP_TEXT_DIALOG_AUTHENTICATING -
		                                                            CREATURE_ISLE_DIALOG_HELP_TEXTS));
		break;
	case DIALUP_STATUS_CONNECTED:
		if (Instance.setup_box != NULL)
		{
			Instance.setup_box->SetOffHold();
			Instance.setup_box->MessageBoxA(
				HelpTextDataBase::HelpTextDatabase.GetHelpText(HELP_TEXT_DIALOG_CONNECTIONESTABLISHED -
			                                                   CREATURE_ISLE_DIALOG_HELP_TEXTS),
				MSGBOXSTYLE_SINGLE_BUTTON, 0);
		}
		Connected = true;
		break;
	case DIALUP_STATUS_AUTHENTICATED:
		wcscpy(text, HelpTextDataBase::HelpTextDatabase.GetHelpText(HELP_TEXT_DIALOG_AUTHENTICATINGOK -
		                                                            CREATURE_ISLE_DIALOG_HELP_TEXTS));
		break;
	case DIALUP_STATUS_AUTHENTICATION_ERROR:
		Connected = false;
		if (Instance.setup_box != NULL)
		{
			Instance.setup_box->SetOffHold();
			Instance.setup_box->MessageBoxA(HelpTextDataBase::HelpTextDatabase.GetHelpText(
												HELP_TEXT_DIALOG_AUTHENTICATINGERROR - CREATURE_ISLE_DIALOG_HELP_TEXTS),
			                                MSGBOXSTYLE_SINGLE_BUTTON, 0);
		}
		break;
	case DIALUP_STATUS_LOGGING_ON:
		wcscpy(text, HelpTextDataBase::HelpTextDatabase.GetHelpText(HELP_TEXT_DIALOG_LOGGINGON -
		                                                            CREATURE_ISLE_DIALOG_HELP_TEXTS));
		break;
	case DIALUP_STATUS_ERROR:
		Connected = false;
		if (Instance.setup_box != NULL)
		{
			Instance.setup_box->SetOffHold();
			Instance.setup_box->MessageBoxA(HelpTextDataBase::HelpTextDatabase.GetHelpText(
												HELP_TEXT_DIALOG_ERRORHASOCCURED - CREATURE_ISLE_DIALOG_HELP_TEXTS),
			                                MSGBOXSTYLE_SINGLE_BUTTON, 0);
		}
		break;
	case DIALUP_STATUS_DISCONNECTED:
		Connected = false;
		if (Instance.setup_box != NULL)
		{
			Instance.setup_box->SetOffHold();
			Instance.setup_box->MessageBoxA(HelpTextDataBase::HelpTextDatabase.GetHelpText(
												HELP_TEXT_DIALOG_DISCONNECTED - CREATURE_ISLE_DIALOG_HELP_TEXTS),
			                                MSGBOXSTYLE_SINGLE_BUTTON, 0);
		}
		break;
	case DIALUP_STATUS_PASSWORD_EXPIRED:
		Connected = false;
		if (Instance.setup_box != NULL)
		{
			Instance.setup_box->SetOffHold();
			Instance.setup_box->MessageBoxA(HelpTextDataBase::HelpTextDatabase.GetHelpText(
												HELP_TEXT_DIALOG_PASSWORDEXPIRED - CREATURE_ISLE_DIALOG_HELP_TEXTS),
			                                MSGBOXSTYLE_SINGLE_BUTTON, 0);
		}
		break;
	}
}

void MPFEChooseConnection::Init(uint32_t background_style, uint32_t tall_background,
                                void(__stdcall* callback)(int, SetupBox*, SetupControl*, int, int))
{
	DialogBoxBase::Init(background_style, tall_background, callback);

	BackButton = new (MPFE_CHOOSE_CONNECTION_FILE, 185) SetupBigButton(
		MPFE_CHOOSE_CONNECTION_CONTROL_ID_BACK, 30, 530, L"", 40, SETUP_TEXT_POSITION_RIGHT, BBSTYLE_LEFT_ARROW);
	BackButton->ContinueButtonCallback = OnClickBack;
	ContinueButton = new (MPFE_CHOOSE_CONNECTION_FILE, 187) SetupBigButton(
		MPFE_CHOOSE_CONNECTION_CONTROL_ID_CONTINUE, 730, 530, L"", 40, SETUP_TEXT_POSITION_LEFT, BBSTYLE_RIGHT_ARROW);
	ContinueButton->ContinueButtonCallback = OnClickContinue;

	TitleText = new (MPFE_CHOOSE_CONNECTION_FILE, 190)
		SetupStaticText(MPFE_CHOOSE_CONNECTION_CONTROL_ID_TITLE, 40, 70, 720, 40, L"", TEXTJUSTIFY_CENTRE);
	TitleText->text_size = GetBigTextSize();

	ConnectionList =
		new (MPFE_CHOOSE_CONNECTION_FILE, 193) SetupList(MPFE_CHOOSE_CONNECTION_CONTROL_ID_LIST, 150, 200, 500, 240);
	ConnectionList->ContinueButtonCallback = OnClickConnectionBox;
	ConnectionList->text_size = GetBigTextSize();

	HiddenEdit = new (MPFE_CHOOSE_CONNECTION_FILE, 196)
		SetupEdit(MPFE_CHOOSE_CONNECTION_CONTROL_ID_EDIT, 260, 471, 280, 30, L"", false);
	HiddenEdit->text_size = GetMidTextSize();
	HiddenEdit->Hide(true);
}

static void OnClickBack(SetupBox* box, SetupControl* control)
{
	MPFEConnectionStatus::Status.LeaveConnectionScreen();
}

static void OnClickContinue(SetupBox* box, SetupControl* control)
{
	if (control->hidden)
	{
		return;
	}
	MPFEConnectionType* connection = (MPFEConnectionType*)MPFEChooseConnection::Instance.ConnectionList->GetItemData(
		MPFEChooseConnection::Instance.ConnectionList->GetSelected());
	if (connection == NULL)
	{
		return;
	}
	LHLobby::SendFullChecksum = false;
	if (connection->Type == MPFE_CONNECTION_TYPE_LAN)
	{
		LHLobby::SendFullChecksum = true;
		MPFEConnectionStatus::Status.SetConnection(connection);
		return;
	}
	if (MPFEConnectionType::ModemConnected() != NULL)
	{
		MPFEConnectionStatus::Status.SetConnection(connection);
		return;
	}
	box->MessageBoxA(L"", MSGBOXSTYLE_OK_CANCEL, 0);
	MPFEChooseConnection::Instance.DialBox = box;
	if (!connection->DialModem(box->MessageText->label))
	{
		box->MessageBoxA(HelpTextDataBase::HelpTextDatabase.GetHelpText(HELP_TEXT_DIALOG_DIALUPERROR -
		                                                                CREATURE_ISLE_DIALOG_HELP_TEXTS),
		                 MSGBOXSTYLE_SINGLE_BUTTON, 0);
	}
}

static void OnClickConnectionBox(SetupBox* box, SetupControl* control)
{
	if (MPFEChooseConnection::Instance.ConnectionList->GetSelected() == -1)
	{
		MPFEChooseConnection::Instance.ContinueButton->Hide(true);
	}
	else
	{
		MPFEChooseConnection::Instance.ContinueButton->Hide(false);
	}
}

void MPFEChooseConnection::Destroy() {}

void MPFEChooseConnection::InitControls()
{
	unsigned long forceLan = 0;
	RegistryRetrieveULong(MPFE_CHOOSE_CONNECTION_REGISTRY_KEY, "ForceLan", &forceLan);
	if (forceLan != 0 && !AskedForIPAddress)
	{
		TimIPSpecial(L"Enter IP Address to connect to", NULL);
		AskedForIPAddress = true;
	}

	field_0x430 = 0;
	wcscpy(BackButton->label, HelpTextData::GetTextL(HELP_TEXT_DIALOG_BACK - CREATURE_ISLE_DIALOG_HELP_TEXTS));
	wcscpy(ContinueButton->label, HelpTextData::GetTextL(HELP_TEXT_DIALOG_ADDITION_96 - CREATURE_ISLE_HELP_TEXTS));
	MPFEConnectionType::InitialiseConnectionList();
	wcscpy(TitleText->label, HelpTextDataBase::HelpTextDatabase.GetHelpText(HELP_TEXT_MPFE_TITLE_CHOOSE_CONNECTION -
	                                                                        CREATURE_ISLE_MPFE_HELP_TEXTS));

	int index = 1;
	ConnectionList->DeleteAll();
	ConnectionList->SelectedIndex = -1;
	bool selected = false;
	Instance.ContinueButton->Hide(true);

	for (LHLinkedListIterator<MPFEConnectionType*> it = MPFEConnectionType::ConnectionList.GetStart(); it; it++)
	{
		MPFEConnectionType* connection = it.Get();
		if (connection->Type == MPFE_CONNECTION_TYPE_LAN)
		{
			ConnectionList->AddString(LHSPrintfW(L"%d.%s", index,
			                                     HelpTextDataBase::HelpTextDatabase.GetHelpText(
													 HELP_TEXT_DIALOG_LANSTRING - CREATURE_ISLE_DIALOG_HELP_TEXTS)),
			                          0, 0, 0);
			selected = true;
			Instance.ContinueButton->Hide(false);
			ConnectionList->SelectedIndex = ConnectionList->NumItems - 1;
		}
		else if (connection->IsDefault)
		{
			ConnectionList->AddString(
				connection->Connected
					? (wchar_t*)LHSPrintfW(L"%d.%s %s %s", index,
			                               HelpTextDataBase::HelpTextDatabase.GetHelpText(
											   HELP_TEXT_DIALOG_CONNECTED - CREATURE_ISLE_DIALOG_HELP_TEXTS),
			                               connection->Name,
			                               HelpTextDataBase::HelpTextDatabase.GetHelpText(
											   HELP_TEXT_DIALOG_DEFAULT - CREATURE_ISLE_DIALOG_HELP_TEXTS))
					: (wchar_t*)LHSPrintfW(L"%d. %s %s", index, connection->Name,
			                               HelpTextDataBase::HelpTextDatabase.GetHelpText(
											   HELP_TEXT_DIALOG_DEFAULT - CREATURE_ISLE_DIALOG_HELP_TEXTS)),
				0, 0, 0);
			if (!selected)
			{
				ConnectionList->SelectedIndex = ConnectionList->NumItems - 1;
				selected = true;
				Instance.ContinueButton->Hide(false);
			}
		}
		else
		{
			ConnectionList->AddString(
				connection->Connected
					? (wchar_t*)LHSPrintfW(L"%d.%s %s", index + 1,
			                               HelpTextDataBase::HelpTextDatabase.GetHelpText(
											   HELP_TEXT_DIALOG_CONNECTED - CREATURE_ISLE_DIALOG_HELP_TEXTS),
			                               connection->Name)
					: (wchar_t*)LHSPrintfW(L"%d. %s", index, connection->Name),
				0, 0, 0);
		}
		ConnectionList->SetItemData(Instance.ConnectionList->NumItems - 1, (uint32_t)connection);
		index++;
	}

	if (MPFEConnectionType::ModemConnected() != NULL)
	{
		MPFEConnectionStatus::Status.SetConnection(MPFEConnectionType::ModemConnected());
	}
}

void __stdcall MPFEChooseConnection::ControlCallback(int message, SetupBox* box, SetupControl* control, int data1,
                                                     int data2)
{
	switch (message)
	{
	case SETUP_MESSAGE_PRE_DRAW: {
		int alpha = SetupThing::DrawAlpha;
		SetupThing::DrawAlpha = alpha / 2;
		SetupThing::DrawBox(Instance.TitleText->rect.start.x, Instance.TitleText->rect.start.y,
		                    Instance.TitleText->rect.end.x, Instance.TitleText->rect.end.y,
		                    MPFE_CHOOSE_CONNECTION_COLOUR, MPFE_CHOOSE_CONNECTION_COLOUR, MPFE_CHOOSE_CONNECTION_COLOUR,
		                    MPFE_CHOOSE_CONNECTION_COLOUR, 1, 1);
		SetupThing::DrawAlpha = alpha;
		break;
	}
	case SETUP_MESSAGE_MESSAGE_BOX_RESULT:
		OnMessageBox(box, control, data1);
		break;
	case SETUP_MESSAGE_ACTIVATE:
		Instance.InitControls();
		break;
	case SETUP_MESSAGE_CLICK:
		if (control != NULL && control->ContinueButtonCallback != NULL)
		{
			control->ContinueButtonCallback(box, control);
		}
		break;
	case SETUP_MESSAGE_UPDATE:
		Instance.ContinueButton->Hide(Instance.ConnectionList->GetItemData(Instance.ConnectionList->GetSelected()) ==
		                              0);
		break;
	}
}

static void OnMessageBox(SetupBox* box, SetupControl* control, int button)
{
	if (button == SETUP_MESSAGE_BOX_ID_CANCEL)
	{
		if (MPFEConnectionType::DialupPresent())
		{
			((MPFEConnectionType*)MPFEChooseConnection::Instance.ConnectionList->GetItemData(
				 MPFEChooseConnection::Instance.ConnectionList->GetSelected()))
				->HangupModem();
		}
	}
	else if (button == SETUP_MESSAGE_BOX_ID_OK)
	{
		if (MPFEChooseConnection::Instance.Connected)
		{
			MPFEConnectionStatus::Status.SetConnection(
				(MPFEConnectionType*)MPFEChooseConnection::Instance.ConnectionList->GetItemData(
					MPFEChooseConnection::Instance.ConnectionList->GetSelected()));
		}
	}
}

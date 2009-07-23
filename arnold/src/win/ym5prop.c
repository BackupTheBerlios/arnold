#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include "../cpc/dumpym.h"
#include "../cpc/messages.h"

extern TCHAR *AYOutputPath;
extern TCHAR *AppendChar(TCHAR *pPtr, const TCHAR ch);

extern TCHAR *AddExtensions(TCHAR *pPtr, const TCHAR **pExtensionList);
extern TCHAR *AppendString(TCHAR *pPtr, const TCHAR *pDescription);

const TCHAR *YM_Extensions[]=
{
	_T("ym"),
	NULL
};


static int YMVersion = 5;
static BOOL YMRecordWhenSilenceEnds = TRUE;
static BOOL YMStopRecordWhenSilenceEnds = FALSE;

void YM_Refresh(HWND hDialog)
{
	char Buffer[256];
	HWND hItem = GetDlgItem(hDialog,IDC_STATIC_RECORDED);
	int nVBL = YMOutput_GetVBL();
	float nSeconds = nVBL/50.0f;
	float nMinutes = floor(nSeconds/60.0f);

	SetCheckButtonState(hDialog, IDC_RADIO_YM3, (YMVersion==3));
	SetCheckButtonState(hDialog, IDC_RADIO_YM5, (YMVersion==5));
	SetCheckButtonState(hDialog, IDC_CHECK_RECORDING_FLAG1, YMRecordWhenSilenceEnds);
	SetCheckButtonState(hDialog, IDC_CHECK_RECORDING_FLAG2, YMStopRecordWhenSilenceEnds);

	nSeconds = floor(nSeconds - (nMinutes*60.0f));

	sprintf(Buffer,"%02d:%02d recorded", (int)nMinutes, (int)nSeconds);
	SetWindowText(hItem, Buffer);

    // disable button if recording.
    hItem = GetDlgItem(hDialog, IDC_RECORD);
    EnableWindow(hItem, !YMOutput_IsEnabled());

    // enable button if recording.
    hItem = GetDlgItem(hDialog, IDC_STOP);
    EnableWindow(hItem, YMOutput_IsEnabled());

    UpdateWindow(hDialog);
}

HWND hYMProperties = NULL;

BOOL	CALLBACK YM5_Dialog_Proc(HWND hDialog, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_INITDIALOG:
		{
			char *Text;

			Text = YMOutput_GetName();

			if (Text!=NULL)
			{
				SetDlgItemText(hDialog, IDC_EDIT_NAME, Text);
			}

			Text = YMOutput_GetAuthor();

			if (Text!=NULL)
			{
				SetDlgItemText(hDialog, IDC_EDIT_AUTHOR, Text);
			}

			Text = YMOutput_GetComment();

			if (Text!=NULL)
			{
				SetDlgItemText(hDialog, IDC_EDIT_COMMENT, Text);
			}
		}
		break;

		case WM_COMMAND:
		{
			// depending on ID selected
			switch (LOWORD(wParam))
			{
				case IDC_CHECK_RECORDING_FLAG1:
				{
					YMRecordWhenSilenceEnds = !YMRecordWhenSilenceEnds;
					YM_Refresh(hDialog);
				}
				break;

				case IDC_CHECK_RECORDING_FLAG2:
				{
					YMStopRecordWhenSilenceEnds = !YMStopRecordWhenSilenceEnds;
					YM_Refresh(hDialog);
				}
				break;

				case IDC_RADIO_YM3:
				{
					YMVersion = 3;
					YM_Refresh(hDialog);
				}
				break;

				case IDC_RADIO_YM5:
				{
					YMVersion = 5;
					YM_Refresh(hDialog);
				}
				break;

				case IDC_RECORD:
				{
					int StartRecordingWhenSilenceEnds = SendDlgItemMessage(hDialog,LOWORD(IDC_CHECK_RECORDING_FLAG1), BM_GETCHECK,0,0);
					int StopRecordingWhenSilenceBegins = SendDlgItemMessage(hDialog,LOWORD(IDC_CHECK_RECORDING_FLAG2), BM_GETCHECK,0,0);

					YMOutput_StartRecording((StartRecordingWhenSilenceEnds!=0),(StopRecordingWhenSilenceBegins!=0));

                    YM_Refresh(hDialog);
				}
				break;

				case IDC_STOP:
				{
					YMOutput_StopRecording();

                    YM_Refresh(hDialog);
				}
				break;

				case IDC_SAVE:
				{
					TCHAR DialogItemTextBuffer[256];
					TCHAR FilenameBuffer[256];
					OPENFILENAME	AYOpenFilename;
					TCHAR			FilesOfType[256];

					FilenameBuffer[0] = _T('\0');

					{
						TCHAR *pPtr;


						pPtr = FilesOfType;

						pPtr = AppendString(pPtr, Messages[30]);
						pPtr = AddExtensions(pPtr, YM_Extensions);
						pPtr = AppendChar(pPtr,_T('\0'));
						pPtr = AppendChar(pPtr,_T('\0'));
					}


					InitFileDlg(hDialog,&AYOpenFilename,_T("ym"),FilesOfType,0);
					if (GetSaveNameFromDlg(hDialog,&AYOpenFilename,NULL,Messages[5],FilenameBuffer, AYOutputPath))
					{
						/* set name */
						GetDlgItemText(hDialog, IDC_EDIT_NAME, DialogItemTextBuffer, 256);
						YMOutput_SetName(DialogItemTextBuffer);

						/* set author */
						GetDlgItemText(hDialog, IDC_EDIT_AUTHOR, DialogItemTextBuffer, 256);
						YMOutput_SetAuthor(DialogItemTextBuffer);

						/* set comment */
						GetDlgItemText(hDialog, IDC_EDIT_COMMENT, DialogItemTextBuffer, 256);
						YMOutput_SetComment(DialogItemTextBuffer);

						{
							unsigned long OutputSize = YM_GetOutputSize(YMVersion);
							unsigned char *pOutputData = malloc(OutputSize);
							if (pOutputData!=NULL)
							{
								YM_GenerateOutputData(pOutputData, YMVersion);
								SaveFile(FilenameBuffer, pOutputData, OutputSize);
								free(pOutputData);
							}
						}
					}
				}
				break;
			}
		}
		break;

		case WM_CLOSE:
			DestroyWindow(hDialog);
			hYMProperties = NULL;
			break;

	}

	return FALSE;
}

void	YM5_PropertiesDialog(HWND hParent)
{
    if (hYMProperties==NULL)
    {
        HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);

        hYMProperties = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG_YM5), hParent, YM5_Dialog_Proc);
    }

    if (hYMProperties!=NULL)
    {
        ShowWindow(hYMProperties, SW_SHOW);
        YM_Refresh(hYMProperties);
    }
}

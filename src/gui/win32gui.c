#include "common.h"
#include "gamma.h"
#include "options.h"
#include "systemtime.h"
#include "solar.h"
#include "gui/win32gui_gamma.h"
#include <CommCtrl.h>
#include "win32/resource.h"

static HWND gHmain;

void guimain_update_info(void){
	double now;
	char buf[BUFSIZ];
	if ( !systemtime_get_time(&now) ){
		LOG(LOGERR,_("Unable to read system time."));
	}else{
		float lat=opt_get_lat();
		float lon=opt_get_lon();
		double elevation;
		/* Current angular elevation of the sun */
		elevation = solar_elevation(now,lat,lon);
		LOG(LOGVERBOSE,_("Elevation - now: %f"),
				elevation);
		//_set_sun_pos(elevation);
	}

	sprintf(buf,_("%d K"),guigamma_get_temp());
	SetWindowText(GetDlgItem(gHmain,IDC_MAIN_ST_CURR),buf);
	sprintf(buf,_("%d K"),opt_get_temp_day());
	SetWindowText(GetDlgItem(gHmain,IDC_MAIN_ST_DAY),buf);
	sprintf(buf,_("%d K"),opt_get_temp_night());
	SetWindowText(GetDlgItem(gHmain,IDC_MAIN_ST_NIGHT),buf);
	sprintf(buf,_("%.2f Lat, %.2f Lon"),opt_get_lat(),opt_get_lon());
	SetWindowText(GetDlgItem(gHmain,IDC_MAIN_ST_LOC),buf);
}


INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg){
		case WM_INITDIALOG:
			gHmain = hDlg;
			guigamma_init_timers();
			if( opt_get_disabled() ){
				guigamma_disable();
			}else{
				guigamma_check((HWND)NULL,(UINT)NULL,(UINT)NULL,(DWORD)NULL);
			}
		case WM_COMMAND:
			switch(LOWORD(wParam)){
			case IDCANCEL:
				SendMessage(hDlg, WM_CLOSE, 0, 0);
				return TRUE;
			}
			break;
		case WM_CLOSE:
			guigamma_end_timers();
			DestroyWindow(hDlg);
			return TRUE;

		case WM_DESTROY:
			PostQuitMessage(0);
			return TRUE;
	}
	return FALSE;
}

// Main GUI code
int win32_gui(int argc, char *argv[]){
	MSG msg;
	BOOL ret;

	InitCommonControls();
	CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MAINDIALOG ), 0, DialogProc, 0);
	ShowWindow(gHmain, SW_SHOW);

	while((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
		if(ret == -1){
			LOG(LOGERR,_("An error occurred."));
			return RET_FUN_FAILED;
		}

		if(!IsDialogMessage(gHmain, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return RET_FUN_SUCCESS;
}
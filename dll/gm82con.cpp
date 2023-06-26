#include "gm82con.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpvReserved) {
    return TRUE;
}

namespace {
	HANDLE g_hConsoleOutput = INVALID_HANDLE_VALUE;
	HANDLE g_hConsoleInput = INVALID_HANDLE_VALUE;
	HANDLE g_hInputThread = INVALID_HANDLE_VALUE;
	DWORD g_dwSignaledHandlerMask = gm82con_event_none;
	DWORD g_dwReturnHandlerMask = (gm82con_event_ctrl_c | gm82con_event_ctrl_close);
	DWORD g_dwOutputConsoleMode =
		(
			ENABLE_PROCESSED_OUTPUT |
			ENABLE_WRAP_AT_EOL_OUTPUT |
			ENABLE_VIRTUAL_TERMINAL_PROCESSING /* VT100 */
		);
	DWORD g_dwInputConsoleMode = 
		(
			ENABLE_ECHO_INPUT |
			ENABLE_INSERT_MODE |
			ENABLE_LINE_INPUT |
			ENABLE_MOUSE_INPUT |
			ENABLE_QUICK_EDIT_MODE |
			ENABLE_EXTENDED_FLAGS |
			ENABLE_PROCESSED_INPUT |
			ENABLE_WINDOW_INPUT
		);

	/* allocate in .bss plzplzplz */
	volatile bool g_bRunThread;
	char g_tmpInputBuffer[8192];

	void __gm82con_kill_handles(void) {
		/* ignore any errors here */

		g_bRunThread = false;
		if (g_hInputThread != INVALID_HANDLE_VALUE) {
			CancelSynchronousIo(g_hInputThread);
			WaitForSingleObject(g_hInputThread, INFINITE);
			CloseHandle(g_hInputThread);
			g_hInputThread = INVALID_HANDLE_VALUE;
			SecureZeroMemory(g_tmpInputBuffer, sizeof(g_tmpInputBuffer));
		}

		if (g_hConsoleOutput != INVALID_HANDLE_VALUE) {
			FlushFileBuffers(g_hConsoleOutput);
			CloseHandle(g_hConsoleOutput);
			g_hConsoleOutput = INVALID_HANDLE_VALUE;
		}

		if (g_hConsoleInput != INVALID_HANDLE_VALUE) {
			FlushFileBuffers(g_hConsoleInput);
			CloseHandle(g_hConsoleInput);
			g_hConsoleInput = INVALID_HANDLE_VALUE;
		}
	}

	BOOL WINAPI __gm82con_handler_routine(DWORD dwCtrlType) {
		BOOL bIsHandled = (g_dwReturnHandlerMask & (1 << dwCtrlType)) != 0;
		if (bIsHandled == TRUE) {
			/* the app handles this signal in it's own way */
			g_dwSignaledHandlerMask |= (1 << dwCtrlType);
		}
		/* otherwise return FALSE (system default handling) */

		return bIsHandled;
	}

	void __gm82con_obtain_handles(void) {
		__gm82con_kill_handles();

		g_hConsoleOutput = CreateFileW(
			L"CONOUT$",
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);

		g_hConsoleInput = CreateFileW(
			L"CONIN$",
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
		
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleCP(CP_UTF8);
		SetConsoleMode(g_hConsoleOutput, g_dwOutputConsoleMode);
		SetConsoleMode(g_hConsoleInput, g_dwInputConsoleMode);
		/* every AllocConsole resets both io handles and routines */
		SetConsoleCtrlHandler(&__gm82con_handler_routine, TRUE);
	}

	DWORD WINAPI __gm82con_input_thread(LPVOID lpvThreadParameter) {
		UNREFERENCED_PARAMETER(lpvThreadParameter);

		while (g_bRunThread) {
			DWORD dwReadChars = 0;
			/* good enough for a single line (if using line input) */
			WCHAR wcTmp[128] = { L'\0' };

			BOOL bResult = ReadConsoleW(
				g_hConsoleInput,
				wcTmp,
				sizeof(wcTmp) / sizeof(wcTmp[0]),
				&dwReadChars,
				nullptr
			);

			if (bResult == TRUE && dwReadChars > 0) {
				/* 128 times 4 should be good enough for UTF-8 */
				char utf8tmp[512] = { '\0' };
				int iWroteBytes = WideCharToMultiByte(
					CP_UTF8,
					WC_ERR_INVALID_CHARS,
					wcTmp,
					dwReadChars,
					utf8tmp,
					sizeof(utf8tmp),
					nullptr,
					nullptr
				);
				if (iWroteBytes > 0) {
					strncat_s(g_tmpInputBuffer, utf8tmp, iWroteBytes);
				}
			}
		}

		return 0;
	}
}

realfunc gm82con_alloc(void) {
	BOOL bResult = FALSE;

	bResult = AllocConsole();
	if (bResult == TRUE) {
		__gm82con_obtain_handles();        
        HWND hwnd = GetConsoleWindow();
        DWORD style = GetWindowLong(hwnd, GWL_STYLE);
        style &= ~WS_MAXIMIZEBOX;
        SetWindowLong(hwnd, GWL_STYLE, style);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_FRAMECHANGED);
        EnableMenuItem(GetSystemMenu(hwnd,false), SC_CLOSE, MF_GRAYED);
	}

	return bResult;
}

realfunc gm82con_free(void) {
	BOOL bResult = FALSE;
	
	__gm82con_kill_handles();
	bResult = FreeConsole();

	return bResult;
}

realfunc gm82con_write_string(gmstring lineString) {
	BOOL bResult = FALSE;

	if (g_hConsoleOutput != INVALID_HANDLE_VALUE && lineString != nullptr) {
		int len=MultiByteToWideChar(
			CP_UTF8,
			MB_ERR_INVALID_CHARS,
			lineString,
			-1,
			NULL,
			0
		);
        
        WCHAR wcBuffer[4096] = { L'\0' };
		int iWroteChars = MultiByteToWideChar(
			CP_UTF8,
			MB_ERR_INVALID_CHARS,
			lineString,
			strlen(lineString),
			wcBuffer,
			len
		);

		if (iWroteChars >= 0) {
			bResult = WriteConsoleW(
				g_hConsoleOutput,
				wcBuffer,
				static_cast<DWORD>(iWroteChars),
				nullptr,
				nullptr
			);
		}
	}

	return bResult;
}

realfunc gm82con_handle_events(gmreal signalmaskReal) {
	DWORD dwPreviousMask = g_dwReturnHandlerMask;
	DWORD dwInputMask = static_cast<DWORD>(signalmaskReal);
	g_dwReturnHandlerMask = dwInputMask;
	/* return the previous mask */
	return static_cast<gmreal>(dwPreviousMask);
}

realfunc gm82con_is_event_signaled(gmreal signalmaskReal) {
	DWORD dwInputMask = static_cast<DWORD>(signalmaskReal);
	DWORD dwResult = g_dwSignaledHandlerMask & dwInputMask;
	/* cancel out the events that have been requested */
	g_dwSignaledHandlerMask &= ~dwInputMask;
	return static_cast<gmreal>(dwResult);
}

realfunc gm82con_input_start(void) {
	if (g_hInputThread != INVALID_HANDLE_VALUE) {
		/* input has already started */
		return true;
	}

	DWORD dwThreadId = 0;
	SecureZeroMemory(g_tmpInputBuffer, sizeof(g_tmpInputBuffer));
	g_bRunThread = true;
	g_hInputThread = CreateThread(
		nullptr,
		0,
		&__gm82con_input_thread,
		nullptr,
		0,
		&dwThreadId
	);

	return true;
}

strfunc gm82con_input_peek(void) {
	return g_tmpInputBuffer;
}

realfunc gm82con_input_clear(void) {
	SecureZeroMemory(g_tmpInputBuffer, sizeof(g_tmpInputBuffer));
	return true;
}

realfunc gm82con_input_stop(void) {
	if (g_hInputThread != INVALID_HANDLE_VALUE) {
		g_bRunThread = false;
		CancelSynchronousIo(g_hInputThread);
		WaitForSingleObject(g_hInputThread, INFINITE);
		CloseHandle(g_hInputThread);
		g_hInputThread = INVALID_HANDLE_VALUE;
		SecureZeroMemory(g_tmpInputBuffer, sizeof(g_tmpInputBuffer));
		return true;
	}

	return false;
}

realfunc gm82con_set_console_modes(gmreal outputReal, gmreal inputReal) {
	BOOL bResult = TRUE;

	if (outputReal >= 0.0) {
		g_dwOutputConsoleMode = static_cast<DWORD>(outputReal);
		if (g_hConsoleOutput != INVALID_HANDLE_VALUE) {
			bResult = bResult && SetConsoleMode(g_hConsoleOutput, g_dwOutputConsoleMode);
		}
	}

	if (inputReal >= 0.0) {
		g_dwInputConsoleMode = static_cast<DWORD>(inputReal);
		if (g_hConsoleInput != INVALID_HANDLE_VALUE) {
			bResult = bResult && SetConsoleMode(g_hConsoleInput, g_dwInputConsoleMode);
		}
	}

	return bResult;
}

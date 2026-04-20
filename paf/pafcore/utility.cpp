#include "utility.h"
#include "std_string.h"
#include "std_unordered_set.h"
#include <assert.h>
#include <mutex>

extern "C" int strcmp(const char* lhs, const char* rhs);

#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif

#if defined(_WIN32)
#include <windows.h>
#include "resource.h"
#endif

BEGIN_PAFCORE

size_t stringBinarySearch(const char** strs, size_t stride, size_t count, const char* value)
{
	size_t left = 0;
	size_t right = count;
	while (left < right)
	{
		size_t mid = (left + right) / 2;
		int_t cmp = strcmp(value, *reinterpret_cast<const char**>(reinterpret_cast<size_t>(strs) + stride * mid));
		if (0 == cmp)
		{
			return mid;
		}
		if (cmp < 0)
		{
			right = mid;
		}
		else
		{
			left = mid + 1;
		}
	}
	return count;
}

void* allocateMemory(size_t n)
{
	return pafcore::Allocate(n);
}

void freeMemory(void* p)
{
	pafcore::Deallocate(p);
}

void DummyDestroyInstance(void* address)
{}

void DummyDestroyArray(void* address)
{}

void DummyAssign(void* dst, const void* src)
{}

END_PAFCORE

#if defined(_WIN32)
static BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam)
{
	if (IsWindowVisible(hwnd))
	{
		HWND* mainWnd = (HWND*)lParam;
		*mainWnd = hwnd;
		return FALSE;
	}
	return TRUE;
}

struct PafAssertionInfo
{
	const wchar_t* condition;
	const wchar_t* file;
	const wchar_t* line;
	const wchar_t* description;
};

class IgnoredAssertion
{
public:
	IgnoredAssertion()
	{
	}
public:
	void ignoreAll(const wchar_t* file, const wchar_t* line)
	{
		pafcore::wstring str = file;
		str += L':';
		str += line;
		m_mutex.lock();
		m_fileAndLines.insert(str);
		m_mutex.unlock();
	}
	bool isIgnoreAll(const wchar_t* file, const wchar_t* line)
	{
		pafcore::wstring str = file;
		str += L':';
		str += line;
		bool res = false;
		m_mutex.lock();
		auto it = m_fileAndLines.find(str);
		res = (it != m_fileAndLines.end());
		m_mutex.unlock();
		return res;
	}
	void ignoreOne(const wchar_t* file, const wchar_t* line, const wchar_t* description)
	{
		pafcore::wstring str = file;
		str += L':';
		str += line;
		str += L':';
		str += description;
		m_mutex.lock();
		m_fileAndLineAndDescriptions.insert(str);
		m_mutex.unlock();
	}
	bool isIgnoreOne(const wchar_t* file, const wchar_t* line, const wchar_t* description)
	{
		pafcore::wstring str = file;
		str += L':';
		str += line;
		str += L':';
		str += description;
		bool res = false;
		m_mutex.lock();
		auto it = m_fileAndLineAndDescriptions.find(str);
		res = (it != m_fileAndLineAndDescriptions.end());
		m_mutex.unlock();
		return res;
	}

private:
	pafcore::unordered_set<pafcore::wstring> m_fileAndLines;
	pafcore::unordered_set<pafcore::wstring> m_fileAndLineAndDescriptions;
	std::mutex m_mutex;
};

IgnoredAssertion g_ignoredAssertion;

static INT_PTR CALLBACK PafAssertionDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			PafAssertionInfo* assertionInfo = (PafAssertionInfo*)lParam;
			SetWindowLongPtr(hDlg, DWLP_USER, (LONG_PTR)assertionInfo);
			SetDlgItemText(hDlg, IDC_EDIT_CONDITION, assertionInfo->condition);
			SetDlgItemText(hDlg, IDC_EDIT_FILE, assertionInfo->file);
			SetDlgItemText(hDlg, IDC_EDIT_LINE, assertionInfo->line);
			SetDlgItemText(hDlg, IDC_EDIT_DESCRIPTION, assertionInfo->description);
			break;
		}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		case IDC_BUTTON_CONTINUE:
			EndDialog(hDlg, IDOK);
			return TRUE;
		case IDC_BUTTON_IGNORE:
			{
				PafAssertionInfo* assertionInfo = (PafAssertionInfo*)GetWindowLongPtr(hDlg, DWLP_USER);
				g_ignoredAssertion.ignoreOne(assertionInfo->file, assertionInfo->line, assertionInfo->description);
				EndDialog(hDlg, IDOK);
				return TRUE;
			}
		case IDC_BUTTON_IGNORE_ALL:
			{
				PafAssertionInfo* assertionInfo = (PafAssertionInfo*)GetWindowLongPtr(hDlg, DWLP_USER);
				g_ignoredAssertion.ignoreAll(assertionInfo->file, assertionInfo->line);
				EndDialog(hDlg, IDOK);
				return TRUE;
			}
		case IDC_BUTTON_BREAK:
			EndDialog(hDlg, IDOK);
			__debugbreak();
			return TRUE;
		case IDC_BUTTON_STOP:
			EndDialog(hDlg, IDOK);
			ExitProcess(0);
			return TRUE;
		}
	}
	return FALSE;
}

void PafAssertionDialog(const wchar_t* condition, const wchar_t* file, unsigned line, const wchar_t* description)
{
	wchar_t strLine[64];
	swprintf(strLine, sizeof(strLine) / sizeof(strLine[0]), L"%u", line);
	if (g_ignoredAssertion.isIgnoreAll(file, strLine))
	{
		return;
	}
	if (g_ignoredAssertion.isIgnoreOne(file, strLine, description))
	{
		return;
	}
	extern DWORD g_pafcore_mainThreadID;
	extern HINSTANCE g_pafcore_instance;
	HWND mainWnd = NULL;

	if (::GetCurrentThreadId() == g_pafcore_mainThreadID)
	{
		EnumThreadWindows(g_pafcore_mainThreadID, EnumThreadWndProc, (LPARAM)&mainWnd);
	}
	PafAssertionInfo assertInfo;
	assertInfo.condition = condition;
	assertInfo.file = file;
	assertInfo.line = strLine;
	assertInfo.description = description;
	DialogBoxParamW(g_pafcore_instance, (LPCWSTR)IDD_ASSERTION_DIALOG, mainWnd, &PafAssertionDialogProc, (LPARAM)&assertInfo);
}
#endif

void PafAssert(wchar_t const* condition, wchar_t const* file, unsigned line, wchar_t const* format, ...)
{
#ifdef _DEBUG
	wchar_t description[1024];
	va_list args;
	va_start(args, format);
	vswprintf(description, sizeof(description) / sizeof(description[0]), format, args);
	va_end(args);
#if defined(_WIN32) && defined(_MSC_VER)
	_wassert(condition, file, line);
#else
	fwprintf(stderr, L"%ls\n%ls(%u): %ls\n", condition, file, line, description);
	abort();
#endif
#else
	(void)condition;
	(void)file;
	(void)line;
	(void)format;
#endif
}

void* PAFCORE_CDECL operator new(size_t size, const char* fileName, int line, int)
{
	(void)fileName;
	(void)line;
	return pafcore::Allocate(size);
}

void* PAFCORE_CDECL operator new[](size_t size, const char* fileName, int line, int)
{
	(void)fileName;
	(void)line;
	return pafcore::Allocate(size);
}

void PAFCORE_CDECL operator delete(void* block, const char* fileName, int line, int)
{
	(void)fileName;
	(void)line;
	pafcore::Deallocate(block);
}

void PAFCORE_CDECL operator delete[](void* block, const char* fileName, int line, int)
{
	(void)fileName;
	(void)line;
	pafcore::Deallocate(block);
}


#include "clar_libgit2.h"
#include "path.h"

#ifdef GIT_WIN32
#include "win32/path_w32.h"
#endif

void test_utf8_to_utf16(const char *utf8_in, const wchar_t *utf16_expected)
{
#ifdef GIT_WIN32
	git_win32_path path_utf16;
	int path_utf16len;

	cl_assert((path_utf16len = git_win32_path_from_utf8(path_utf16, utf8_in)) >= 0);
	cl_assert_equal_wcs(utf16_expected, path_utf16);
	cl_assert_equal_i(wcslen(utf16_expected), path_utf16len);
#else
	GIT_UNUSED(utf8_in);
	GIT_UNUSED(utf16_expected);
#endif
}

void test_path_win32__utf8_to_utf16(void)
{
#ifdef GIT_WIN32
	test_utf8_to_utf16("C:\\", L"\\\\?\\C:\\");
	test_utf8_to_utf16("c:\\", L"\\\\?\\c:\\");
	test_utf8_to_utf16("C:/", L"\\\\?\\C:\\");
	test_utf8_to_utf16("c:/", L"\\\\?\\c:\\");
#endif
}

void test_path_win32__removes_trailing_slash(void)
{
#ifdef GIT_WIN32
	test_utf8_to_utf16("C:\\Foo\\", L"\\\\?\\C:\\Foo");
	test_utf8_to_utf16("C:\\Foo\\\\", L"\\\\?\\C:\\Foo");
	test_utf8_to_utf16("C:\\Foo\\\\", L"\\\\?\\C:\\Foo");
	test_utf8_to_utf16("C:/Foo/", L"\\\\?\\C:\\Foo");
	test_utf8_to_utf16("C:/Foo///", L"\\\\?\\C:\\Foo");
#endif
}

void test_path_win32__squashes_multiple_slashes(void)
{
#ifdef GIT_WIN32
	test_utf8_to_utf16("C:\\\\Foo\\Bar\\\\Foobar", L"\\\\?\\C:\\Foo\\Bar\\Foobar");
	test_utf8_to_utf16("C://Foo/Bar///Foobar", L"\\\\?\\C:\\Foo\\Bar\\Foobar");
#endif
}

void test_path_win32__unc(void)
{
#ifdef GIT_WIN32
	test_utf8_to_utf16("\\\\", L"\\\\?\\UNC\\");
	test_utf8_to_utf16("\\\\server\\c$\\unc\\path", L"\\\\?\\UNC\\server\\c$\\unc\\path");
	test_utf8_to_utf16("//server/git/style/unc/path", L"\\\\?\\UNC\\server\\git\\style\\unc\\path");
#endif
}

void test_path_win32__honors_max_path(void)
{
#ifdef GIT_WIN32
	git_win32_path path_utf16;

	cl_git_pass(git_libgit2_opts(GIT_OPT_SET_WINDOWS_LONGPATHS, 0));
	test_utf8_to_utf16("C:\\This path is 259 chars and is the max length in windows\\0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij",
		L"\\\\?\\C:\\This path is 259 chars and is the max length in windows\\0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij");
	test_utf8_to_utf16("\\\\unc\\paths may also be 259 characters including the server\\123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij",
		L"\\\\?\\UNC\\unc\\paths may also be 259 characters including the server\\123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij");
	cl_check_fail(git_win32_path_from_utf8(path_utf16, "C:\\This path is 260 chars and is sadly too long for windows\\0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij"));
	cl_check_fail(git_win32_path_from_utf8(path_utf16, "\\\\unc\\paths are also bound by 260 character restrictions\\including the server name portion\\bcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij"));

	cl_git_pass(git_libgit2_opts(GIT_OPT_SET_WINDOWS_LONGPATHS, 1));
	test_utf8_to_utf16("C:\\This path is 260 chars but is fine for windows\\0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789", L"\\\\?\\C:\\This path is 260 chars but is fine for windows\\0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789");
	test_utf8_to_utf16("\\\\unc\\260 character unc paths are also fine for windows\\including the server name portion\\bcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0", L"\\\\?\\UNC\\unc\\260 character unc paths are also fine for windows\\including the server name portion\\bcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0");
	cl_check_fail(git_win32_path_from_utf8(path_utf16, "C:\\This path is 4096 chars and is sadly too long for git\\0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghi"));
	cl_check_fail(git_win32_path_from_utf8(path_utf16, "\\\\unc\\paths are also bound by 4096 character restrictions\\including the server name portion\\bcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcde"));
#endif
}

void test_path_win32__dot_and_dotdot(void)
{
#ifdef GIT_WIN32
	test_utf8_to_utf16("C:\\Foo\\..\\Foobar", L"\\\\?\\C:\\Foobar");
	test_utf8_to_utf16("C:\\Foo\\Bar\\..\\Foobar", L"\\\\?\\C:\\Foo\\Foobar");
	test_utf8_to_utf16("C:\\Foo\\Bar\\..\\Foobar\\..", L"\\\\?\\C:\\Foo");
	test_utf8_to_utf16("C:\\Foobar\\..", L"\\\\?\\C:\\");
	test_utf8_to_utf16("C:/Foo/Bar/../Foobar", L"\\\\?\\C:\\Foo\\Foobar");
	test_utf8_to_utf16("C:/Foo/Bar/../Foobar/../Asdf/", L"\\\\?\\C:\\Foo\\Asdf");
	test_utf8_to_utf16("C:/Foo/Bar/../Foobar/..", L"\\\\?\\C:\\Foo");
	test_utf8_to_utf16("C:/Foo/..", L"\\\\?\\C:\\");

	test_utf8_to_utf16("C:\\Foo\\Bar\\.\\Foobar", L"\\\\?\\C:\\Foo\\Bar\\Foobar");
	test_utf8_to_utf16("C:\\.\\Foo\\.\\Bar\\.\\Foobar\\.\\", L"\\\\?\\C:\\Foo\\Bar\\Foobar");
	test_utf8_to_utf16("C:/Foo/Bar/./Foobar", L"\\\\?\\C:\\Foo\\Bar\\Foobar");
	test_utf8_to_utf16("C:/Foo/../Bar/./Foobar/../", L"\\\\?\\C:\\Bar");

	test_utf8_to_utf16("C:\\Foo\\..\\..\\Bar", L"\\\\?\\C:\\Bar");
#endif
}

void test_path_win32__absolute_from_no_drive_letter(void)
{
#ifdef GIT_WIN32
	test_utf8_to_utf16("\\Foo", L"\\\\?\\C:\\Foo");
	test_utf8_to_utf16("\\Foo\\Bar", L"\\\\?\\C:\\Foo\\Bar");
	test_utf8_to_utf16("/Foo/Bar", L"\\\\?\\C:\\Foo\\Bar");
#endif
}

void test_path_win32__absolute_from_relative(void)
{
#ifdef GIT_WIN32
	char cwd_backup[MAX_PATH];

	cl_must_pass(p_getcwd(cwd_backup, MAX_PATH));
	cl_must_pass(p_chdir("C:/"));

	test_utf8_to_utf16("Foo", L"\\\\?\\C:\\Foo");
	test_utf8_to_utf16("..\\..\\Foo", L"\\\\?\\C:\\Foo");
	test_utf8_to_utf16("Foo\\..", L"\\\\?\\C:\\");
	test_utf8_to_utf16("Foo\\..\\..", L"\\\\?\\C:\\");
	test_utf8_to_utf16("", L"\\\\?\\C:\\");

	cl_must_pass(p_chdir("C:/Windows"));

	test_utf8_to_utf16("Foo", L"\\\\?\\C:\\Windows\\Foo");
	test_utf8_to_utf16("Foo\\Bar", L"\\\\?\\C:\\Windows\\Foo\\Bar");
	test_utf8_to_utf16("..\\Foo", L"\\\\?\\C:\\Foo");
	test_utf8_to_utf16("Foo\\..\\Bar", L"\\\\?\\C:\\Windows\\Bar");
	test_utf8_to_utf16("", L"\\\\?\\C:\\Windows");

	cl_must_pass(p_chdir(cwd_backup));
#endif
}

#ifdef GIT_WIN32
static void test_canonicalize(const wchar_t *in, const wchar_t *expected)
{
	git_win32_path canonical;

	cl_assert(wcslen(in) < MAX_PATH);
	wcscpy(canonical, in);

	cl_must_pass(git_win32_path_canonicalize(canonical));
	cl_assert_equal_wcs(expected, canonical);
}
#endif

static void test_remove_namespace(const wchar_t *in, const wchar_t *expected)
{
#ifdef GIT_WIN32
	git_win32_path canonical;

	cl_assert(wcslen(in) < MAX_PATH);
	wcscpy(canonical, in);

	git_win32_path_remove_namespace(canonical, wcslen(in));
	cl_assert_equal_wcs(expected, canonical);
#else
	GIT_UNUSED(in);
	GIT_UNUSED(expected);
#endif
}

void test_path_win32__remove_namespace(void)
{
	test_remove_namespace(L"\\\\?\\C:\\Temp\\Foo", L"C:\\Temp\\Foo");
	test_remove_namespace(L"\\\\?\\C:\\", L"C:\\");
	test_remove_namespace(L"\\\\?\\", L"");

	test_remove_namespace(L"\\??\\C:\\Temp\\Foo", L"C:\\Temp\\Foo");
	test_remove_namespace(L"\\??\\C:\\", L"C:\\");
	test_remove_namespace(L"\\??\\", L"");

	test_remove_namespace(L"\\\\?\\UNC\\server\\C$\\folder", L"\\\\server\\C$\\folder");
	test_remove_namespace(L"\\\\?\\UNC\\server\\C$\\folder", L"\\\\server\\C$\\folder");
	test_remove_namespace(L"\\\\?\\UNC\\server\\C$", L"\\\\server\\C$");
	test_remove_namespace(L"\\\\?\\UNC\\server\\", L"\\\\server");
	test_remove_namespace(L"\\\\?\\UNC\\server", L"\\\\server");

	test_remove_namespace(L"\\??\\UNC\\server\\C$\\folder", L"\\\\server\\C$\\folder");
	test_remove_namespace(L"\\??\\UNC\\server\\C$\\folder", L"\\\\server\\C$\\folder");
	test_remove_namespace(L"\\??\\UNC\\server\\C$", L"\\\\server\\C$");
	test_remove_namespace(L"\\??\\UNC\\server\\", L"\\\\server");
	test_remove_namespace(L"\\??\\UNC\\server", L"\\\\server");

	test_remove_namespace(L"\\\\server\\C$\\folder", L"\\\\server\\C$\\folder");
	test_remove_namespace(L"\\\\server\\C$", L"\\\\server\\C$");
	test_remove_namespace(L"\\\\server\\", L"\\\\server");
	test_remove_namespace(L"\\\\server", L"\\\\server");

	test_remove_namespace(L"C:\\Foo\\Bar", L"C:\\Foo\\Bar");
	test_remove_namespace(L"C:\\", L"C:\\");
	test_remove_namespace(L"", L"");

}

void test_path_win32__canonicalize(void)
{
#ifdef GIT_WIN32
	test_canonicalize(L"C:\\Foo\\Bar", L"C:\\Foo\\Bar");
	test_canonicalize(L"C:\\Foo\\", L"C:\\Foo");
	test_canonicalize(L"C:\\Foo\\\\", L"C:\\Foo");
	test_canonicalize(L"C:\\Foo\\..\\Bar", L"C:\\Bar");
	test_canonicalize(L"C:\\Foo\\..\\..\\Bar", L"C:\\Bar");
	test_canonicalize(L"C:\\Foo\\..\\..\\..\\..\\", L"C:\\");
	test_canonicalize(L"C:/Foo/Bar", L"C:\\Foo\\Bar");
	test_canonicalize(L"C:/", L"C:\\");

	test_canonicalize(L"Foo\\\\Bar\\\\Asdf\\\\", L"Foo\\Bar\\Asdf");
	test_canonicalize(L"Foo\\\\Bar\\\\..\\\\Asdf\\", L"Foo\\Asdf");
	test_canonicalize(L"Foo\\\\Bar\\\\.\\\\Asdf\\", L"Foo\\Bar\\Asdf");
	test_canonicalize(L"Foo\\\\..\\Bar\\\\.\\\\Asdf\\", L"Bar\\Asdf");
	test_canonicalize(L"\\", L"");
	test_canonicalize(L"", L"");
	test_canonicalize(L"Foo\\..\\..\\..\\..", L"");
	test_canonicalize(L"..\\..\\..\\..", L"");
	test_canonicalize(L"\\..\\..\\..\\..", L"");

	test_canonicalize(L"\\\\?\\C:\\Foo\\Bar", L"\\\\?\\C:\\Foo\\Bar");
	test_canonicalize(L"\\\\?\\C:\\Foo\\Bar\\", L"\\\\?\\C:\\Foo\\Bar");
	test_canonicalize(L"\\\\?\\C:\\\\Foo\\.\\Bar\\\\..\\", L"\\\\?\\C:\\Foo");
	test_canonicalize(L"\\\\?\\C:\\\\", L"\\\\?\\C:\\");
	test_canonicalize(L"//?/C:/", L"\\\\?\\C:\\");
	test_canonicalize(L"//?/C:/../../Foo/", L"\\\\?\\C:\\Foo");
	test_canonicalize(L"//?/C:/Foo/../../", L"\\\\?\\C:\\");

	test_canonicalize(L"\\\\?\\UNC\\server\\C$\\folder", L"\\\\?\\UNC\\server\\C$\\folder");
	test_canonicalize(L"\\\\?\\UNC\\server\\C$\\folder\\", L"\\\\?\\UNC\\server\\C$\\folder");
	test_canonicalize(L"\\\\?\\UNC\\server\\C$\\folder\\", L"\\\\?\\UNC\\server\\C$\\folder");
	test_canonicalize(L"\\\\?\\UNC\\server\\C$\\folder\\..\\..\\..\\..\\share\\", L"\\\\?\\UNC\\server\\share");

	test_canonicalize(L"\\\\server\\share", L"\\\\server\\share");
	test_canonicalize(L"\\\\server\\share\\", L"\\\\server\\share");
	test_canonicalize(L"\\\\server\\share\\\\foo\\\\bar", L"\\\\server\\share\\foo\\bar");
	test_canonicalize(L"\\\\server\\\\share\\\\foo\\\\bar", L"\\\\server\\share\\foo\\bar");
	test_canonicalize(L"\\\\server\\share\\..\\foo", L"\\\\server\\foo");
	test_canonicalize(L"\\\\server\\..\\..\\share\\.\\foo", L"\\\\server\\share\\foo");
#endif
}

void test_path_win32__8dot3_name(void)
{
#ifdef GIT_WIN32
	char *shortname;

	if (!cl_sandbox_supports_8dot3())
		clar__skip();

	/* Some guaranteed short names */
	cl_assert_equal_s("PROGRA~1", (shortname = git_win32_path_8dot3_name("C:\\Program Files")));
	git__free(shortname);

	cl_assert_equal_s("WINDOWS", (shortname = git_win32_path_8dot3_name("C:\\WINDOWS")));
	git__free(shortname);

	/* Create some predictible short names */
	cl_must_pass(p_mkdir(".foo", 0777));
	cl_assert_equal_s("FOO~1", (shortname = git_win32_path_8dot3_name(".foo")));
	git__free(shortname);

	cl_git_write2file("bar~1", "foobar\n", 7, O_RDWR|O_CREAT, 0666);
	cl_must_pass(p_mkdir(".bar", 0777));
	cl_assert_equal_s("BAR~2", (shortname = git_win32_path_8dot3_name(".bar")));
	git__free(shortname);
#endif
}

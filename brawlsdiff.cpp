#include "brawlls.h"
#include <windows.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::IO;

int brawlsdiff(String^ filename1, String^ filename2, int console_width) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	if (console_width <= 0 && columns > 0) console_width = columns;

	String^ f1 = Path::GetTempFileName();
	String^ f2 = Path::GetTempFileName();
	StreamWriter^ s1 = gcnew StreamWriter(gcnew FileStream(f1, FileMode::Create, FileAccess::Write));
	StreamWriter^ s2 = gcnew StreamWriter(gcnew FileStream(f2, FileMode::Create, FileAccess::Write));
	array<String^>^ args1 = gcnew array<String^>(2);
	array<String^>^ args2 = gcnew array<String^>(2);
	args1[0] = args2[0] = "-Rm";
	args1[1] = filename1;
	args2[1] = filename2;
	int r1 = brawlls(args1, s1);
	if (r1 != 0) return r1;
	int r2 = brawlls(args2, s2);
	if (r2 != 0) return r2;
	delete s1;
	delete s2;
	ProcessStartInfo psi;
	psi.FileName = "sdiff";
	psi.Arguments = "--strip-trailing-cr -t -s " + f1 + " " + f2;
	if (console_width >= 0) psi.Arguments += " -w " + console_width;
	psi.UseShellExecute = false;
	//psi.EnvironmentVariables["CYGWIN"] += "nodosfilewarning";
	int exitcode;
	try {
		Process^ p = Process::Start(%psi);
		p->WaitForExit();
		exitcode = p->ExitCode;
	} catch (System::ComponentModel::Win32Exception^ e) {
		Console::Error->WriteLine("Error: exception thrown: " + e->Message);
		Console::Error->WriteLine("Maybe sdiff.exe isn't in your PATH?");
		exitcode = 1;
	}
	File::Delete(f1);
	File::Delete(f2);
	return exitcode;
}
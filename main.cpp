#include "brawlls.h"
#include "brawlsdiff.h"

int main(array<System::String^>^ args) {
	if (args->Length > 0 && args[0] == "sdiff") return compare(args);

	return brawlls(args, System::Console::Out);
}
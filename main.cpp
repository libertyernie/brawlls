#include "brawlls.h"
#include "brawlsdiff.h"

int main(array<System::String^>^ args) {
	for each(System::String^ argument in args) {
		if (argument == "sdiff") return compare(args);
	}

	return brawlls(args, System::Console::Out);
}
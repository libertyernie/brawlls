using namespace System;

int brawlls(array<String^>^ args);
int compare(array<String^>^ args);

int main(array<String^>^ args) {
	if (args->Length > 0 && args[0] == "sdiff") return compare(args);

	String^ file_to_delete = nullptr;
	int r = brawlls(args);
	return r;
}
int brawlls(array<System::String^>^ args);
int compare(array<System::String^>^ args);

int main(array<System::String^>^ args) {
	if (args->Length > 0 && args[0] == "sdiff") return compare(args);

	return brawlls(args);
}
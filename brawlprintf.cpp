#include "isinst.h"
#include <string>
#include <iostream>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Reflection;
using namespace System::Text;
using namespace BrawlLib::SSBB::ResourceNodes;

String^ format_obj(String^ format, String^ prefix, Object^ obj) {
	String^ name = obj == nullptr
		? "null"
		: obj->ToString();
	String^ bone = "";
	if (isinst<MDL0BoneNode^>(obj)) {
		MDL0BoneNode^ b = (MDL0BoneNode^)obj;
		bone = "T" + b->Translation;
		bone += " R" + b->Rotation;
		bone += " S" + b->Scale;
	}
	String^ index = "";
	String^ md5 = "";
	String^ size = "";
	String^ path = "";
	if (isinst<ResourceNode^>(obj)) {
		ResourceNode^ node = (ResourceNode^)obj;
		if (format->Contains("%m")) { // don't do this if we don't need the data - this does save some time
			md5 = "MD5:" + node->MD5Str();
		}
		index = node->Index + "";
		size = node->OriginalSource.Length + "";
		while (node->Parent != nullptr) {
			path = node->Name + "/" + path;
			node = node->Parent;
		}
		if (path->EndsWith("/")) path = path->Substring(0, path->Length - 1);
	}
	return format
		->Replace("%~", prefix)
		->Replace("%n", name)
		->Replace("%p", path)
		->Replace("%i", index)
		->Replace("%t", "(" + obj->GetType()->Name + ")")
		->Replace("%b", bone)
		->Replace("%m", md5)
		->Replace("%s", size)
		->Replace("%%", "%");
}

String^ properties_str(String^ prefix, Object^ node) {
	StringBuilder sb;
	for each(PropertyInfo^ entry in node->GetType()->GetProperties()) {
		for each(Attribute^ attribute in entry->GetCustomAttributes(false)) {
			if (isinst<CategoryAttribute^>(attribute)) {
				Object^ val = entry->GetValue(node, nullptr);
				String^ valstr = val == nullptr
					? "null"
					: val->ToString();
				sb.AppendLine(prefix + entry->Name + " " + valstr);
			}
		}
	}
	return sb.ToString();
}

float rv_endian(float input) {
	float output = 0;
	char* ptr1 = (char*)&input;
	char* ptr2 = (char*)&output;
	ptr2[0] = ptr1[3];
	ptr2[1] = ptr1[2];
	ptr2[2] = ptr1[1];
	ptr2[3] = ptr1[0];
	return output;
}

String^ stdt_floats_str(String^ prefix, ResourceNode^ node) {
	StringBuilder sb;
	float* addr = (float*)(void*)node->UncompressedSource.Address;
	int length = node->UncompressedSource.Length;
	for (int i = 5; i < length; i++) {
		float rv = rv_endian(addr[i]);
		sb.AppendLine(prefix +
			"0x" + (i*4).ToString("X2") + ": " +
			rv + " / " + *((__int32*)(&rv)));
	}
	return sb.ToString();
}

bool datatag(const char* tag, ResourceNode^ node) {
	if (node->UncompressedSource.Length < 4) return false;
	return 0 == std::strncmp(tag, (char*)(void*)node->UncompressedSource.Address, 4);
}

String^ details_str(String^ prefix, ResourceNode^ node) {
	if (isinst<STPMEntryNode^>(node)) {
		return properties_str(prefix, node);
	} else if (datatag("STDT", node)) {
		return stdt_floats_str(prefix, node);
	} else {
		return nullptr;
	}
}

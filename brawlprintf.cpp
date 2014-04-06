#include "isinst.h"
#include "values32b.h"

using namespace System;
using System::ComponentModel::CategoryAttribute;
using System::Reflection::PropertyInfo;
using System::Text::StringBuilder;
using System::IO::StringWriter;
using namespace BrawlLib::SSBB::ResourceNodes;

bool data_tag_is(const char* tag, ResourceNode^ node) {
	if (node->UncompressedSource.Length < 4) return false;
	char* ptr = (char*)(void*)node->UncompressedSource.Address;
	for (int i = 0; i < 4; i++) {
		if (ptr[i] != tag[i]) return false;
	}
	return true;
}

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
			md5 = "MD5:" + node->MD5StrEnsureChildrenIncluded();
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

String^ properties_lines(String^ prefix, Object^ node) {
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

String^ details_str(String^ prefix, ResourceNode^ node) {
	if (isinst<STPMEntryNode^>(node)) {
		return properties_lines(prefix, node);
	} else if (isinst<MSBinNode^>(node)) {
		MSBinNode^ msbin = (MSBinNode^)node;
		StringBuilder sb;
		for each(String^ s in msbin->_strings) {
			sb.AppendLine(prefix + s);
		}
		return sb.ToString();
	} else if (data_tag_is("STDT", node)) {
		return values32b_to_clistr(prefix, node);
	} else {
		return nullptr;
	}
}

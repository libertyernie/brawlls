using namespace System;
using namespace System::ComponentModel;
using namespace System::Reflection;
using namespace System::Text;
using namespace BrawlLib::SSBB::ResourceNodes;

template < class T, class U >
Boolean isinst(U u) {
	return dynamic_cast< T >(u) != nullptr;
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

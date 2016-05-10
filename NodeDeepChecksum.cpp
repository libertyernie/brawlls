using namespace BrawlLib::SSBB::ResourceNodes;
using System::Collections::Generic::Queue;
using System::VoidPtr;
using System::String;
using System::Text::StringBuilder;

#include "NodeDeepChecksum.h"

/// <summary>
/// Determines whether all of this node's direct children are contained between the start and end of its own data.
/// If the result is false, an identical MD5() result will not guarantee that the node's children have not changed.
/// </summary>
bool DataSourceContainsAllChildren(ResourceNode^ node) {
	// Some nodes might not contain in their DataSource all of the data of their children
	// I think nodes that don't have their own data (e.g. NW4R folders) are an example of this.

	DataSource^ data = node->OriginalSource;
	VoidPtr paddr = data->Address;
	int plen = data->Length;

	// initialize queue
	Queue<ResourceNode^>^ queue = gcnew Queue<ResourceNode^>(node->Children);
	while (queue->Count > 0) {
		ResourceNode^ c = queue->Dequeue();
		if (c->OriginalSource.Address.address == nullptr || c->OriginalSource.Length == 0) {
			// There is no data in that node, but its children probably have some
			for each (ResourceNode^ c2 in c->Children) {
				queue->Enqueue(c2);
			}
		} else {
			// That node has data
			VoidPtr addr = c->OriginalSource.Address;
			int len = c->OriginalSource.Length;
			if (addr >= paddr && addr < paddr + len) {
				// That node's data is inside of this node's data
			} else {
				// That node's data is separate
				return false;
			}
		}
	}

	return true;
}

/// <summary>
/// Find a checksum of this node's data, ensuring that changes in this
/// node's children are reflected as well.
/// </summary>
/// <returns>An array of 16 bytes</returns>
/// <param name="resultType">A pointer to store the result type, or NULL</param>
/// <remarks>Three cases are possible:
/// * CHILDREN_XOR: This node does not have a data block, and MD5() returns null - result is MD5ChildrenXor()
/// * SELF: All children are included within the data block pointed to by OriginalSource -> result is MD5()
/// * SELF_AND_CHILDREN_XOR: At least one child is not included within the data block -> result is MD5() xor with MD5ChildrenXor()</remarks>
array<unsigned char>^ MD5EnsureChildrenIncluded(ResourceNode^ main, NodeDeepChecksumResultType* resultType) {
	array<unsigned char>^ self = main->MD5();
	if (self == nullptr) {
		if (resultType != nullptr) *resultType = NODE_DEEP_CHECKSUM_CHILDREN_XOR;
		return MD5ChildrenXor(main);
	}
	if (!DataSourceContainsAllChildren(main)) {
		array<unsigned char>^ children = MD5ChildrenXor(main);
		for (int i = 0; i < 16; i++) self[i] ^= children[i];
		if (resultType != nullptr) *resultType = NODE_DEEP_CHECKSUM_SELF_AND_CHILDREN_XOR;
	} else {
		if (resultType != nullptr) *resultType = NODE_DEEP_CHECKSUM_SELF;
	}
	return self;
}

/// <summary>
/// Use an XOR operation to combine the results of running
/// MD5EnsureChildrenIncluded on each child node.
/// </summary>
array<unsigned char>^ MD5ChildrenXor(ResourceNode^ main) {
	bool childrenfound = false;
	array<unsigned char>^ xorsum = gcnew array<unsigned char>(16);
	for each (ResourceNode^ node in main->Children) {
		array<unsigned char>^ md5 = MD5EnsureChildrenIncluded(node, nullptr);
		if (md5 != nullptr) {
			childrenfound = true;
			for (int i = 0; i < 16; i++) xorsum[i] ^= md5[i];
		}
	}
	return childrenfound ? xorsum : nullptr;
}

/// <summary>
/// Get the result of the MD5StrEnsureChildrenIncluded() function as a
/// string of hexadecimal digits. If children's checksums were used in an
/// XOR operation (in which case it's not a true MD5 checksum), a suffix
/// will be appended to the string - either (c) or (s+c).
/// If MD5StrEnsureChildrenIncluded() returns null, this method will
/// return an empty string.
/// </summary>
[System::Runtime::ExceptionServices::HandleProcessCorruptedStateExceptions]
String^ MD5StrEnsureChildrenIncluded(ResourceNode^ main) {
	try {
		NodeDeepChecksumResultType result;
		array<unsigned char>^ checksum = MD5EnsureChildrenIncluded(main, &result);
		if (checksum == nullptr) return String::Empty;
		StringBuilder^ sb = gcnew StringBuilder(checksum->Length * 2 + 5);
		for (int i = 0; i < checksum->Length; i++) {
			sb->Append(checksum[i].ToString("X2"));
		}
		if (result == NODE_DEEP_CHECKSUM_CHILDREN_XOR) {
			sb->Append("(c)");
		} else if (result == NODE_DEEP_CHECKSUM_SELF_AND_CHILDREN_XOR) {
			sb->Append("(s+c)");
		}
		return sb->ToString();
	} catch (System::AccessViolationException^ e) {
		return "----AccessViolationException----";
	}
}

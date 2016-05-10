enum NodeDeepChecksumResultType {
	NODE_DEEP_CHECKSUM_SELF = 1,
	NODE_DEEP_CHECKSUM_CHILDREN_XOR = 2,
	NODE_DEEP_CHECKSUM_SELF_AND_CHILDREN_XOR = 4
}; 

array<unsigned char>^ MD5ChildrenXor(BrawlLib::SSBB::ResourceNodes::ResourceNode^ main);
array<unsigned char>^ MD5EnsureChildrenIncluded(BrawlLib::SSBB::ResourceNodes::ResourceNode^ main, NodeDeepChecksumResultType* resultType);

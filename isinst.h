template < class T, class U >
System::Boolean isinst(U u) {
	return dynamic_cast< T >(u) != nullptr;
}

#include <iostream>

template<typename T>
void swap(T * src, T * dest) {
	T tmp = *src;
	*src = *dest;
	*dest = tmp;
}

template<typename T>
void swap(T * src, T * dest, const int size) {
	T tmp;
	for (int i = 0; i < size; i++) {
		tmp = src[i];
		src[i] = dest[i];
		dest[i] = tmp;
	}
}

int main(void) {
	char src[] = "abc";
	char dest[] = "def";
	
	std::cout << src << ", " << dest << std::endl;
	swap(src, dest, sizeof(src));
	std::cout << src << ", " << dest << std::endl;
}
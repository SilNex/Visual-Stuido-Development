#include <stdio.h>

int main(void) {
	int i = 0, add = 0;
	while (1) {
		printf("i is %d\nAdd: ", i);
		scanf_s("%d", &add);
		i += add;
	}
}
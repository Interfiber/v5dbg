#include <cstdio>

#define $function __current_label:

typedef void(*other_func)();

// Example usage
void otherFunc() {
	$function

	printf("Hello World!\n");

	other_func e = (other_func) &&__current_label;

	e();
}

int main() {
	otherFunc();
}

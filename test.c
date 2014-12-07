#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <error.h>
#include <errno.h>

#include "prompt.h"

void h(int sig) {
	
}

int main() {
	char *line,d;
	int len = 0;
	line = prompt(stdin,stderr);
	printf("%s",line);
	return 0;
}
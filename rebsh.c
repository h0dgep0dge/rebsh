#include <stdio.h>
#include <string.h>

#include "prompt.h"

int isnormal(int c,int escaped) {
	return (!isspace(c)) || escaped;
}

int main(int argc,char** argv) {
	char *b;
	int arg_count = 0;
	// char *o = line_buffer;
	
	while((b = prompt(stdin,stderr," > ",NULL)) != NULL) {
		// unsigned int i = 0;
		// while(i < strlen(line_buffer)) { printf("%i \n",__LINE__);
			// for(;i < strlen(line_buffer);i++) if(!isspace(line_buffer[i])) break;
			// int slashed = 0;
			// int quoted = 0;
			// for(;i < strlen(line_buffer);i++) { printf("%i \n",__LINE__);
				// if(line_buffer[i] == '\\' && slashed == 0) {
					// slashed = 1;
					// continue;
				// }
				// if((line_buffer[i] == '"' || line_buffer[i] == '\'') && slashed == 0) {
					// if(quoted == 0) {
						// quoted = line_buffer[i];
						// continue;
					// }
					// if(quoted == line_buffer[i]) {
						// quoted = 0;
						// continue;
					// }
				// }
				// if(isnormal(line_buffer[i],(slashed != 0) || (quoted != 0))) *(o++) = line_buffer[i];
				// else break;
				// if(slashed > 0) slashed--;
			// }
			// if(i >= strlen(line_buffer)) break;
			// *(o++) = '\0';
			// i++;
		// }
		printf("%s\n",b);
	}
	printf("\n");
	return 0;
}
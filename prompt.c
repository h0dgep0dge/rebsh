#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "prompt.h"

#define BUFFER_INCR 100

char *prompt(FILE *istream, FILE *ostream) {
	char *ps1,*ps2,*prompt,*b;
	int closed = 0,size = 1;
	
	ps1 = getenv("PS1");
	if(ps1 == NULL) ps1 = "> ";
	ps2 = getenv("PS2");
	if(ps2 == NULL) ps2 = "... ";
	
	b = malloc(BUFFER_INCR*size); // Error check me, dammit!
	b[0] = '\0';
	
	prompt = ps1;
	do {
		char *in;
		if((in = linein(istream,ostream,prompt)) == NULL) {
			// Do something about errors here?
			return NULL;
		}
		for(;strlen(in)+strlen(b)+1 > BUFFER_INCR*size;b = realloc(b,(++size)*BUFFER_INCR));
		strncat(b,in,BUFFER_INCR*size);
		free(in);
		prompt = ps2;
	} while(!is_complete(b));
	return b;
}

char *linein(FILE *istream, FILE *ostream,char *prompt) {
	char *b;
	unsigned int chunkc = 1;
	unsigned int linelen = 0;
	unsigned int curserpos = 0;
	unsigned int ttycols;
	struct termios old_settings;
	struct termios new_settings;
	struct winsize w;
	int c;
	
	if(!isatty(fileno(istream))) {
		// Not a tty, maybe return an error or something?
		return NULL;
	}
	
	ioctl(fileno(istream), TIOCGWINSZ, &w);
	ttycols = w.ws_col;
	
	b = malloc(BUFFER_INCR*chunkc); // Error check me, dammit!
	fprintf(ostream,"%s",prompt);
	fflush(ostream);
	
	if(tcgetattr(fileno(istream),&old_settings) < 0) {
		error(1,errno,"tcgetattr");
		return 0;
	}
	memcpy(&new_settings,&old_settings,sizeof(new_settings));
	new_settings.c_lflag ^= new_settings.c_lflag&ECHO;
	new_settings.c_lflag ^= new_settings.c_lflag&ICANON;
	tcsetattr(fileno(istream),TCSANOW,&new_settings);
	
	do {
		c = fgetc(istream);
		
		if(c < 0) {
			// Error checking
			return NULL;
		}
		
		if(c == 27) { // Beginning of an escape code
			int c = fgetc(istream);
			if(c == '[') {// Multi-character code
				char code[20];
				char *i = code;
				do {
					int c = fgetc(istream);
					*(i++) = c;
				} while(c < 64 && c > 126);
				*i = '\0';
				if(strcmp(code,"C") == 0 && curserpos < linelen) { // Right arrow key
					if((strlen(prompt)+curserpos+1)%ttycols == 0) {
						fputc('\n',ostream);
						fputc('\r',ostream);
					} else {
						fputc(27,ostream);
						fputc('[',ostream);
						fputc('C',ostream);
					}
					curserpos++;
				}
				if(strcmp(code,"D") == 0 && curserpos > 0) { // Left arrow key
					// fputc(27,ostream);
					// fputc('[',ostream);
					// fputc('D',ostream);
					fputc('\b',ostream);
					curserpos--;
				}
			} else { // Two character code
				// Have your way with 'c' now.
			}
			continue;
		}
		
		if(BUFFER_INCR*chunkc-linelen < 2) {
			chunkc++;
			b = realloc(b,BUFFER_INCR*chunkc); // Error check me dammit!!
		}
		memmove(b+curserpos+1,b+curserpos,linelen-curserpos);
		*(b+curserpos) = c;
		{
			unsigned int i;
			for(i = curserpos;i <= linelen;i++) fputc(*(b+i),ostream);
			for(;i > curserpos+1;i--) fputc('\b',ostream);
		}
		linelen++;
		curserpos++;
	} while(c != 10);
	tcsetattr(fileno(istream),TCSANOW,&old_settings);
	*(b+linelen) = '\0';
	return b;
}

int is_complete(char *line) {
	int slashed = 0;
	int quoted = 0;
	unsigned int i;
	for(i = 0;i < strlen(line);i++) {
		if(line[i] == '\\' && slashed == 0) {
			slashed = 1;
			continue;
		}
		if((line[i] == '"' || line[i] == '\'') && slashed == 0) {
			if(line[i] == quoted) quoted = 0;
			else if(quoted == 0) quoted = line[i];
		}
		if(line[i] == '\n' && slashed == 0 && quoted == 0) return 1;
		slashed = 0;
	}
	return 0;
}

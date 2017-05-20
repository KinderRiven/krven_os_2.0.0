#include "ui.h"
#include "stdio.h"
#include "string.h"

char welcome_str[] = "Welcome to K'OS!";

void ui_welcome()
{

	int len = strlen(welcome_str);	
	int blank = (80 - len) / 2;	

	ui_line(79);

	printf("\n");
	ui_blank(blank);

	printc(c_black, c_light_red, "%s", welcome_str);
	
	printf("\n");
	ui_line(79);
	printf("\n");
}

void ui_line(int num)
{
	while(num--){
		printf("-");
	}
}

void ui_blank(int num)
{

	while(num--){
		printf(" ");
	}
}

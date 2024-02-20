#define TUI_ACTION_ESC 		0
#define TUI_ACTION_ENTER 	1
#define TUI_ACTION_TAB 		2

char *tui_drawbox (char *org_text, int *action);
char *tui_textbox (int y, char *caption, char *org_text, int max_lines, int *action);
void tui_general_box (char *message, char *alt, char symbol);
void tui_alert (char *message, char *alt);
int tui_yes_or_no (char *message, char *alt);

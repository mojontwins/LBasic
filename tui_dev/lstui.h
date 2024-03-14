// LSTUI v0.1

/*
	To use, #define LSTUI_CONTROL_IMPLEMENTATION before #include "lstui.h" in 
	one of the source files in your project.

	call `lstui_reset ();` before starting, and `lstui_shutdown ()` when you're
	done.

	create your controls, then add them to the list using `lstui_add (c)`.

	in your loop, call `lstui_do ()` to update & handle controls in the list.
*/

#include <stdlib.h>
#include "../dos-like/source/dos.h"

#define LSTUI_STATE_DISABLED 				1
#define LSTUI_STATE_HOVERED 				2

// Main structure used by controls. 

typedef struct LSTUI_CONTROL {
	int x, y; 									// top-left corner position
	int w, h; 									// width / height
	int (*update_func)(LSTUI_CONTROL); 			// Pointer to a function to update this control
	int (*hover_func)(LSTUI_CONTROL);		 	// Pointer to a function to handle hover on this control
	int (*click_func)(LSTUI_CONTROL); 			// Pointer to a function to handle click on this control
	int (*render_func)(LSTUI_CONTROL);			// Pointer to a function to render this control
	int state; 									// Stores basic state
	char *input; 								// Serialized keyboard input (chars)
	char *data; 								// Pointer to related data (input buffer, caption...)
} LSTUI_CONTROL;

// General functions

int lstui_reset (void);							// Reset / inits
int lstui_add (LSTUI_CONTROL c); 				// Add a new control
int lstui_do (void); 							// Iterates the controls and does stuff once
int lstui_free (LSTUI_CONTROL c); 				// Frees a control
int lstui_shutdown (void); 						// Cleans up everything

// Built-in controls

// Get a new simple caption

// Get a new button
struct LSTUI_CONTROL lstui_button (int x, int y, int w, int h, char *caption);

// Get a new text box

#ifdef LSTUI_CONTROL_IMPLEMENTATION

int controls_number = 0;
int focused_control = 0;
LSTUI_CONTROL *controls;

// Internal backend shit

#define MOUSEB_LEFT 1
#define MOUSEB_RIGHT 2
int mouse_x, mouse_y, mouse_b;
int left_click_was = 0;
int right_click_was = 0;

int get_mouse_x (void) {
	mouse_x = mousex ();
	if (mouse_x < 0) mouse_x = 0;
	if (mouse_x >= screenwidth ()) mouse_x = screenwidth () - 1;
	return mouse_x;
}

int get_mouse_y (void) {
	mouse_y = mousey ();
	if (mouse_y < 0) mouse_y = 0;
	if (mouse_y >= screenheight ()) mouse_y = screenheight () - 1;
	return mouse_y;
}

int get_mouse_b (int button) {
	if (button & MOUSEB_LEFT) return keystate (KEY_LBUTTON);
	if (button & MOUSEB_RIGHT) return keystate (KEY_RBUTTON);
	return 0;
}

// Stuff

int lstui_reset (void) {
	if (controls_number) lstui_shutdown ();
	controls_number = 0;
}

int lstui_add (LSTUI_CONTROL c) {
	int index = controls_number;
	controls_number ++;
	controls = realloc (controls, controls_number * sizeof (LSTUI_CONTROL));
	
	controls [index] = c;
	controls [index].state = 0;
}

int lstui_do (void) {
	// Collect key & mouse status

	int mx = get_mouse_x ();
	int my = get_mouse_y (); 

	int left_click = 0;
	int right_click = 0;
	int b;

	b = get_mouse_b (MOUSEB_LEFT);
	if (b && !left_click_was) {
		left_click = 1;
	}
	left_click_was = b;

	b = get_mouse_b (MOUSEB_RIGHT);
	if (b && !right_click_was) {
		right_click = 1;
	}
	right_click_was = b;

	// Keep track of which control is focused

	// For each control
	for (int i = 0; i < controls_number; i ++) {
		LSTUI_CONTROL c = controls [i];
		int over_control = mx >= c.x && mx < c.x + c.w && my >= c.y && my < c.y + c;

		// Pass collected input

		// Update function
		if (c.update_func != NULL && focused_control == i) c.update_func (c);

		// Hover function
		if (c.hover_func != NULL &&	over_control) c.hover_func (c);

		// Click function
		if (c.click_func != NULL && over_control && left_click) c.click_func (c);

		// Render function
		if (c.render_func != NULL) c.render_func (c);
	}
}

int lstui_free (LSTUI_CONTROL c) {
	free (c.input);
	free (c.data);
	return 1;
}

int lstui_shutdown (void) {
	if (controls == NULL) return 0;
	for (int i = 0; i < controls_number; i ++) {
		lstui_free (controls [i]);
	}
	free (controls);

	return 1;
}

/*
 * LSTUI_BUTTON
 */

int lstui_button_hover (LSTUI_CONTROL me) {

}

int lstui_button_click (LSTUI_CONTROL me) {

}

int lstui_button_render (LSTUI_CONTROL me) {

}

struct LSTUI_CONTROL lstui_button (
	int x, int y, int w, int h,
	char *caption
) {
	LSTUI_CONTROL c;
	c.x = x; c.y = y; c.w = w, c.h = h;
	c.update_func = NULL;
	c.hover_func = lstui_button_hover;
	c.click_func = lstui_button_click;
	c.render_func = lstui_button_render;
	c.data = caption;

	return c;
}

#endif

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
#include <string.h>
#include "../dos-like/source/dos.h"

#define LSTUI_STATE_DISABLED                1
#define LSTUI_STATE_HOVERED                 2
#define LSTUI_STATE_CLICKED                 4

#define LSTUI_ALIGN_CENTER                  0
#define LSTUI_ALIGN_LEFT                    1
#define LSTUI_ALIGN_RIGHT                   2

// Main structure used by controls. 

typedef struct LSTUI_CONTROL {
	int x, y; 									// top-left corner position
	int w, h; 									// width / height
	int (*update_func)(int idx); 				// Pointer to a function to update this control
	int (*hover_func)(int idx, int over);		// Pointer to a function to handle hover on this control
	int (*click_func)(int idx, int clicked); 	// Pointer to a function to handle click on this control
	int (*render_func)(int idx, int focused);	// Pointer to a function to render this control
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
	controls = NULL;
}

int lstui_add (LSTUI_CONTROL c) {
	int index = controls_number;
	controls_number ++;
	controls = realloc (controls, controls_number * sizeof (LSTUI_CONTROL));
	
	controls [index] = c;
	controls [index].state = 0;

	return index;
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
		int over_control = mx >= c.x && mx < c.x + c.w && my >= c.y && my < c.y + c.h;

		// Pass collected input

		// Update function
		if (c.update_func != NULL && focused_control == i) c.update_func (i);

		// Hover function
		if (c.hover_func != NULL) c.hover_func (i, over_control);

		// Click function
		if (c.click_func != NULL) c.click_func (i, over_control && left_click);

		// Render function
		if (c.render_func != NULL) c.render_func (i, focused_control == i);
	}
}

int lstui_getstate (int control) {
	return controls [control].state;
}

char *lstui_getdata (int control) {
	return controls [control].data;
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
 * GENERAL BACKEND
 */

int lstui_caption_do (int x, int y, int w, int align, int attrib, char *s) {

	int x1;
	switch (align) {
		case LSTUI_ALIGN_CENTER: x1 = x + w / 2 - strlen (s) / 2; break;
		case LSTUI_ALIGN_LEFT:   x1 = x; break;
		case LSTUI_ALIGN_RIGHT:  x1 = x + w - strlen (s) + 1; break;
	}

	unsigned char *ptr = screenbuffer () + 2 * screenwidth () * y + x1 * 2;
	char c;
	while (c = *s ++) {
		*ptr ++ = c;
		*ptr ++ = attrib;
	}
}

int lstui_box (int x1, int y1, int x2, int y2, int double_frame, int attrib) {
	unsigned char *buf = screenbuffer ();
	int scr_w_bytes = screenwidth () * 2;

	char c1, c2, c3, c4, l1, l2;
	if (double_frame) {
		c1 = 0xC9; c2 = 0xC8; c3 = 0xBB; c4 = 0xBC; l1 = 0xCD; l2 = 0xBA;
	} else {
		c1 = 0xDA; c2 = 0xC0; c3 = 0xBF; c4 = 0xD9; l1 = 0xC4; l2 = 0xB3;
	}


	for (int y = y1; y <= y2; y ++) {
		for (int x = x1; x <= x2; x ++) {
			char c = x > x1 && x < x2 ? ((y == y1 || y == y2) ? l1 : 32) : ((y != y1 && y != y2) ? l2 : ((x == x1) ? (y == y1 ? c1 : c2) : (y == y1 ? c3 : c4)));
			int idx = y * scr_w_bytes + x * 2;
			buf [idx] = c;
			buf [idx + 1] = attrib;
		}
	}
}

/*
 * LSTUI_BUTTON
 */

int lstui_button_attrib = 7;
int lstui_button_attrib_hovered = 7 << 4;

int lstui_button_hover (int me, int over) {
	if (!(controls [me].state & LSTUI_STATE_DISABLED)) {
		controls [me].state = over ? controls [me].state | LSTUI_STATE_HOVERED : controls [me].state & ~LSTUI_STATE_HOVERED;
	}
}

int lstui_button_click (int me, int clicked) {
	if (clicked && !(controls [me].state & LSTUI_STATE_DISABLED)) {
		controls [me].state |= LSTUI_STATE_CLICKED;
	} else {
		controls [me].state &= ~LSTUI_STATE_CLICKED;
	}
}

int lstui_button_render (int me, int focused) {
	int attrib = controls [me].state & LSTUI_STATE_HOVERED ? lstui_button_attrib_hovered : lstui_button_attrib;
	lstui_box (controls [me].x, controls [me].y, controls [me].x + controls [me].w - 1, controls [me].y + controls [me].h - 1, focused, attrib);
	lstui_caption_do (controls [me].x, controls [me].y + (controls [me].h - 1) / 2, controls [me].w, LSTUI_ALIGN_CENTER, attrib, controls [me].data);
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

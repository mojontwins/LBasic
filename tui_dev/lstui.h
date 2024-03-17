// LSTUI v0.1

/*
	To use, #define LSTUI_CONTROL_IMPLEMENTATION before #include "lstui.h" in 
	one of the source files in your project.

	call `lstui_reset ();` before starting, and `lstui_shutdown ()` when you're
	done.

	create your controls, then add them to the list using `lstui_add (c)`, 
	which will return an id as a handle.

	in your loop, call `lstui_do ()` to update & handle controls in the list.

	Use lstui_getstate (handle) or lstui_getdata (handle)
*/

#include <stdlib.h>
#include <string.h>
#include "../dos-like/source/dos.h"
#include "../dev/conversion.h"

#define LSTUI_STATE_DISABLED                1
#define LSTUI_STATE_HOVERED                 2
#define LSTUI_STATE_CLICKED                 4

#define LSTUI_ALIGN_CENTER                  0
#define LSTUI_ALIGN_LEFT                    1
#define LSTUI_ALIGN_RIGHT                   2

#define LSTUI_BOX_SINGLE 					0
#define LSTUI_BOX_DOUBLE 					1

#define LSTUI_SIGNAL_NEXT 					32

// Main structure used by controls. 

typedef struct LSTUI_CONTROL {
	int type; 
	int x, y; 									// top-left corner position
	int w, h; 									// width / height
	int (*update_func)(int idx); 				// Pointer to a function to update this control
	int (*hover_func)(int idx, int over);		// Pointer to a function to handle hover on this control
	int (*click_func)(int idx, int clicked); 	// Pointer to a function to handle click on this control
	int (*render_func)(int idx, int focused);	// Pointer to a function to render this control
	int focusable;								// Set to 1 if this control can get focus
	int state; 									// Stores basic state
	int config; 								// This control configuration
	int signal; 								// General purporse
	char *input; 								// Serialized keyboard input (chars)
	enum keycode_t* keys; 						// Special keypresses
	char *data; 								// Pointer to related data (input buffer, caption...)
} LSTUI_CONTROL;

// General functions

int lstui_reset (void);							// Reset / inits
int lstui_add (LSTUI_CONTROL c); 				// Add a new control
int lstui_do (void); 							// Iterates the controls and does stuff once
int lstui_free (LSTUI_CONTROL c); 				// Frees a control
int lstui_shutdown (void); 						// Cleans up everything

// Handle existing controls

int lstui_getstate (int control);
char *lstui_getdata (int control);

// Configure theme (lame)

void lstui_colors_set_input_attribs (int normal, int focused);
void lstui_colors_set_button_attribs (int normal, int hovered);
void lstui_colors_set_checkbox_attribs (int normal, int focused);

// Built-in controls

struct LSTUI_CONTROL lstui_caption (int x, int y, int w, int c1, int c2, int align, char *caption);
struct LSTUI_CONTROL lstui_button (int x, int y, int w, int h, char *caption);
struct LSTUI_CONTROL lstui_input (int x, int y, int w, int text_length, char *text);
struct LSTUI_CONTROL lstui_box (int x, int y, int w, int h,	int c1, int c2, int frame);
struct LSTUI_CONTROL lstui_checkbox (int x, int y, char *caption);

#define LSTUI_CONTROL_TYPE_CAPTION 0
#define LSTUI_CONTROL_TYPE_BUTTON 1
#define LSTUI_CONTROL_TYPE_INPUT 2
#define LSTUI_CONTROL_TYPE_BOX 3
#define LSTUI_CONTROL_TYPE_CHECKBOX 4

// Get a new text box

#ifdef LSTUI_CONTROL_IMPLEMENTATION
#undef LSTUI_CONTROL_IMPLEMENTATION

int controls_number = 0;
int focused_control = 0;
LSTUI_CONTROL *controls;

// Internal backend shit

#define MOUSEB_LEFT 1
#define MOUSEB_RIGHT 2
int mouse_x, mouse_y, mouse_b;
int left_click_was = 0;
int right_click_was = 0;
int lstui_signal;

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

	focused_control = controls_number; 		// Which means no control is focused.

	return index;
}

int lstui_do (void) {
	cursoff ();

	if (controls_number == 0) return;

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

	unsigned char *chars = (unsigned char*) readchars ();

	// Keep track of which control is focused
	int current_focused = focused_control;

	unsigned char *chars_ptr = chars; char c;
	while (c = *chars_ptr ++) {
		if (c == '\t') {
			lstui_signal = LSTUI_SIGNAL_NEXT; 
			break;
		}
	}

	if (lstui_signal & LSTUI_SIGNAL_NEXT) {
		do {
			focused_control ++; 
			if (focused_control >= controls_number) {
				focused_control = 0;
			}
			if (controls [focused_control].focusable && !(controls [focused_control].state & LSTUI_STATE_DISABLED)) break;
		} while (current_focused != focused_control);
	}

	// Encode additional data
	enum keycode_t* keys = readkeys ();

	// Reset signal
	lstui_signal = 0;

	// For each control
	for (int i = 0; i < controls_number; i ++) {
		LSTUI_CONTROL control = controls [i];
		int over_control = mx >= control.x && mx < control.x + control.w && my >= control.y && my < control.y + control.h;
		
		// Click gives focus!			
		if (over_control && left_click && control.focusable) focused_control = i;


		// Update function
		if (control.update_func != NULL && focused_control == i) {
			// Pass collected input
			controls [i].input = chars;
			controls [i].keys = keys;

			// Call update
			control.update_func (i);

			// Special signals
			if (controls [i].signal & LSTUI_SIGNAL_NEXT) {
				lstui_signal |= LSTUI_SIGNAL_NEXT;
			}

			// Just in case
			controls [i].input = NULL;
			controls [i].keys = NULL;
		}

		// Hover function
		if (control.hover_func != NULL) control.hover_func (i, over_control);

		// Click function
		if (control.click_func != NULL) {
			control.click_func (i, over_control && left_click);
		}

		// Render function
		if (control.render_func != NULL) control.render_func (i, focused_control == i);

	}
}

int lstui_getstate (int control) {
	return controls [control].state;
}

void lstui_setstate (int control, int state) {
	controls [control].state = state;
}

char *lstui_getdata (int control) {
	return controls [control].data;
}

void lstui_setdata (int control, char *data) {
	controls [control].data = data;
}

void lstui_gettype (int control) {
	return controls [control].type;
}

void lstui_num_controls (void) {
	return controls_number;
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

void lstui_caption_do (int x, int y, int w, int align, int attrib, char *s) {

	int x1;
	int x2 = x + w;
	switch (align) {
		case LSTUI_ALIGN_CENTER: x1 = x + w / 2 - strlen (s) / 2; break;
		case LSTUI_ALIGN_LEFT:   x1 = x; break;
		case LSTUI_ALIGN_RIGHT:  x1 = x2 - strlen (s) + 1; break;
	}

	unsigned char *ptr = screenbuffer () + 2 * screenwidth () * y + x1 * 2;
	int xx = x1;
	char c;
	while (xx < x2 && (c = *s ++)) {
		*ptr ++ = c;
		*ptr ++ = attrib;
		++ xx;
	}
}

void lstui_box_do (int x1, int y1, int x2, int y2, int double_frame, unsigned char attrib) {
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

void lstui_fill (int x1, int y1, int w, int h, unsigned char attrib, unsigned char c) {
	unsigned char *buf = screenbuffer ();
	int scr_w_bytes = screenwidth () * 2;
	int x2 = x1 + w; int y2 = y1 + h;

	for (int y = y1; y < y2; y ++) {
		unsigned char *ptr = buf + y * scr_w_bytes + x1 * 2;
		for (int x = x1; x < x2; x ++) {
			*ptr ++ = c;
			*ptr ++ = attrib;
		}
	}
}

int lstui_copy_to_length (char *to, char *from, int l) {	
	int i, ll = strlen (from);
	for (i = 0; i < l && i < ll; i ++) {
		to [i] = from [i];
	}
	to [i] = 0;
	return i;
}

/*
 * Default controls: Captions
 */

// config is attrib | align << 8

lstui_caption_render (int me, int focused) {
	LSTUI_CONTROL c = controls [me];
	int attrib = c.config & 0xff;
	lstui_fill (c.x, c.y, c.w, c.h, attrib, 32);
	lstui_caption_do (c.x, c.y + (c.h - 1) / 2, c.w, c.config >> 8, attrib, c.data);
}

struct LSTUI_CONTROL lstui_caption (
	int x, int y, int w, 
	int c1, int c2, int align, char *caption 
) {
	LSTUI_CONTROL c;

	c.type = LSTUI_CONTROL_TYPE_CAPTION;
	c.x = x; c.y = y; c.w = w; c.h = 1;
	c.update_func = NULL;
	c.hover_func = NULL;
	c.click_func = NULL;
	c.render_func = lstui_caption_render;
	c.data = caption;
	int attrib = c1 | c2 << 4;
	c.config = attrib | align << 8;
	c.focusable = c.state = 0;

	return c;
}

/*
 * Default controls: Buttons
 */

int lstui_button_attrib = 7;
int lstui_button_attrib_hovered = 7 << 4;

int lstui_button_update (int me) {
	controls [me].signal = 0;

	unsigned char c, *chars_ptr = controls [me].input;
	while (c = *chars_ptr ++) {
		if (c == '\n' || c == 32) {
			controls [me].signal = 1; 
			break;
		}
	}
}

int lstui_button_hover (int me, int over) {
	if (!(controls [me].state & LSTUI_STATE_DISABLED)) {
		controls [me].state = over ? controls [me].state | LSTUI_STATE_HOVERED : controls [me].state & ~LSTUI_STATE_HOVERED;
	}
}

int lstui_button_click (int me, int clicked) {
	if ((clicked || controls [me].signal) && !(controls [me].state & LSTUI_STATE_DISABLED)) {
		controls [me].state |= LSTUI_STATE_CLICKED;
	} else {
		controls [me].state &= ~LSTUI_STATE_CLICKED;
	}
}

int lstui_button_render (int me, int focused) {
	LSTUI_CONTROL c = controls [me];
	int attrib = c.state & LSTUI_STATE_HOVERED ? lstui_button_attrib_hovered : lstui_button_attrib;
	lstui_box_do (c.x, c.y, c.x + c.w - 1, c.y + c.h - 1, focused, attrib);
	lstui_caption_do (c.x, c.y + (c.h - 1) / 2, c.w, LSTUI_ALIGN_CENTER, attrib, c.data);
}

struct LSTUI_CONTROL lstui_button (
	int x, int y, int w, int h,
	char *caption
) {
	LSTUI_CONTROL c;

	c.type = LSTUI_CONTROL_TYPE_BUTTON;
	c.x = x; c.y = y; c.w = w, c.h = h;
	c.update_func = lstui_button_update;
	c.hover_func = lstui_button_hover;
	c.click_func = lstui_button_click;
	c.render_func = lstui_button_render;
	c.data = caption;
	c.focusable = 1;
	c.state = 0;

	return c;
}

/*
 * Default controls: text input
 */

// Status stores cursor & scroll position as in cursor | scroll << 16
// Config stores max length

int lstui_input_attrib = 7;
int lstui_input_attrib_focused = 14;

int lstui_input_update (int me) {
	char *line_pointer = controls [me].data;
	int cursor = controls [me].state & 0xFFFF;
	int scroll = controls [me].state >> 16;
	controls [me].signal = 0;

	// Process input
	unsigned char *ptr = controls [me].input;
	unsigned char c; while (c = *ptr ++) {
		
		c = translate (c);
		if (c >= ' ') {
			// Insert a new character (end / middle):
			int line_length = strlen (line_pointer);

			// if it fits
			if (line_length < controls [me].config) {
				
				// Make room
				for (int i = line_length; i >= cursor; i --) {
					line_pointer [i + 1] = line_pointer [i];
				}

				// Insert
				line_pointer [cursor] = c;

				cursor ++;
			}

		} else if (c == 8) {
			// Delete
			if (cursor) {
				cursor --;
				for (int i = cursor; i < strlen (line_pointer); i ++) {
					line_pointer [i] = line_pointer [i + 1];
				}
			}
		
		} else if (c == 13) {
			controls [me].signal = LSTUI_SIGNAL_NEXT;

		}
		
	}

	enum keycode_t* keys = controls [me].keys;
	unsigned long long key; while (key = (unsigned long long) *keys ++) {
		if (key == KEY_LEFT) {
			if (cursor > 0) cursor --;
		}

		if (key == KEY_RIGHT) {
			if (cursor < strlen (line_pointer)) cursor ++;
		}
	}

	while (cursor < scroll) { scroll --; }
	while (cursor - scroll > controls [me].w) { scroll ++; }

	controls [me].state = (scroll << 16) | cursor;

	curson ();
	gotoxy (controls [me].x + cursor - scroll, controls [me].y);
}

int lstui_input_render (int me, int focused) {
	LSTUI_CONTROL c = controls [me];
	int attrib = focused ? lstui_input_attrib_focused : lstui_input_attrib;
	int scroll = c.state >> 16;
	lstui_fill (c.x, c.y, c.w, c.h, attrib, ' ');
	lstui_caption_do (c.x, c.y, c.w, LSTUI_ALIGN_LEFT, attrib, c.data + scroll);
}

struct LSTUI_CONTROL lstui_input (
	int x, int y, int w, 
	int text_length, char *text
) {
	LSTUI_CONTROL c;

	c.type = LSTUI_CONTROL_TYPE_INPUT;
	c.x = x; c.y = y; c.w = w; c.h = 1;
	c.update_func = lstui_input_update;
	c.hover_func = NULL;
	c.click_func = NULL;
	c.render_func = lstui_input_render;
	c.data = malloc (1 + text_length);	
	c.config = text_length;
	c.focusable = 1;

	// Copy text
	c.state = lstui_copy_to_length (c.data, text, text_length);

	return c;
}

/*
 * Default controls: just a box
 */

// Config stores frame | attrib >> 8

int lstui_box_render (int me, int focused) {
	LSTUI_CONTROL c = controls [me];
	int frame = c.config & 0xff;
	int attrib = c.config >> 8;
	lstui_box_do (c.x, c.y, c.x + c.w - 1, c.y + c.h - 1, frame, attrib);
}

struct LSTUI_CONTROL lstui_box (
	int x, int y, int w, int h,
	int c1, int c2, int frame
) {
	LSTUI_CONTROL c;
	
	c.type = LSTUI_CONTROL_TYPE_BOX;
	c.x = x; c.y = y; c.w = w; c.h = h;
	c.update_func = NULL;
	c.hover_func = NULL;
	c.click_func = NULL;
	c.render_func = lstui_box_render;
	c.data = NULL;
	c.config = frame | ((c1 | c2 << 4) << 8);
	c.focusable = c.state = 0;

	return c;
}

/*
 * Default control: check box
 */

// state is 0|1

int lstui_checkbox_attrib = 7 + 16;
int lstui_checkbox_attrib_focused = 14;

int lstui_checkbox_update (int me) {
	unsigned char *ptr = controls [me].input;
	unsigned char c; while (c = *ptr ++) {
		if (c == ' ' || c == '\n') {
			controls [me].state ^= 1;
			return;
		}
	}
}

int lstui_checkbox_click (int me, int clicked) {
	if (clicked) controls [me].state ^= 1;
}

int lstui_checkbox_render (int me, int focused) {
	LSTUI_CONTROL c = controls [me];
	lstui_caption_do (c.x, c.y, c.w, LSTUI_ALIGN_LEFT, 
		focused ? lstui_checkbox_attrib_focused : lstui_checkbox_attrib, 
		c.state ? "[\xFE]" : "[ ]"
	);
	lstui_caption_do (c.x + 4, c.y, strlen (c.data), LSTUI_ALIGN_LEFT, lstui_checkbox_attrib, c.data);
}

struct LSTUI_CONTROL lstui_checkbox (
	int x, int y, char *caption
) {
	LSTUI_CONTROL c;

	c.type = LSTUI_CONTROL_TYPE_CHECKBOX;
	c.x = x; c.y = y; c.w = 3; c.h = 1;
	c.update_func = lstui_checkbox_update;
	c.hover_func = NULL;
	c.click_func = lstui_checkbox_click;
	c.render_func = lstui_checkbox_render;
	c.data = caption;
	c.focusable = 1;
	c.state = 0;

	return c;
}

/*
 * Set colors
 */

void lstui_colors_set_input_attribs (int normal, int focused) {
	lstui_input_attrib = normal;
	lstui_input_attrib_focused = focused;
}

void lstui_colors_set_button_attribs (int normal, int hovered) {
	lstui_button_attrib = normal;
	lstui_button_attrib_hovered = hovered;
}

void lstui_colors_set_checkbox_attribs (int normal, int focused) {
	lstui_checkbox_attrib = normal;
	lstui_checkbox_attrib_focused = focused;
}

#endif

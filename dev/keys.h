#define MT_KEY_ENTER   1
#define MT_KEY_ESC     2
#define MT_KEY_UP      4
#define MT_KEY_DOWN    8
#define MT_KEY_LEFT    16
#define MT_KEY_RIGHT   32
#define MT_KEY_YES     64
#define MT_KEY_NO      128
#define MT_KEY_LBUTTON 256
#define MT_KEY_RBUTTON 512

void keys_read (void);

int keys_get (void);
int keys_get_this_frame (void);

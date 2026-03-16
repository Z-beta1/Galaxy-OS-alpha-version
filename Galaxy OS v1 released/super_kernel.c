typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

#define VIDEO_MEM ((uint16_t*)0xB8000)
#define COLS 80
#define ROWS 25

/* Hardware I/O */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

unsigned char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

int x = 0, y = 1;
char command_buffer[64];
int buffer_idx = 0;

/* Simple string comparison for our shell */
int strcmp(char* s1, char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void print(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') { x = 0; y++; continue; }
        VIDEO_MEM[y * COLS + x] = (0x0F << 8) | str[i];
        x++;
    }
}

void backspace() {
    if (buffer_idx > 0 && x > 2) { // Don't delete the "$ " prompt
        buffer_idx--;
        x--;
        VIDEO_MEM[y * COLS + x] = (0x0F << 8) | ' ';
    }
}

void draw_interface() {
    // Clear whole screen
    for (int i = 0; i < ROWS * COLS; i++) VIDEO_MEM[i] = (0x0F << 8) | ' ';
    
    // Top Bar (Blue)
    for (int i = 0; i < COLS; i++) VIDEO_MEM[i] = (0x1F << 8) | ' ';
    char* top = " GALAXY OS v1.0 | Z-ARCH KERNEL ";
    for (int i = 0; top[i] != '\0'; i++) VIDEO_MEM[i] = (0x1F << 8) | top[i];

    // Bottom Status Bar (Dark Grey/Black)
    for (int i = 0; i < COLS; i++) VIDEO_MEM[(ROWS-1)*COLS + i] = (0x08 << 8) | ' ';
    char* bottom = " [F1] Help | [F2] Reboot | root@galaxy ";
    for (int i = 0; bottom[i] != '\0'; i++) VIDEO_MEM[(ROWS-1)*COLS + i] = (0x08 << 8) | bottom[i];

    x = 0; y = 1;
    print("Terminal Active. Type 'ver' or 'cls'\n$ ");
}

void process_command() {
    command_buffer[buffer_idx] = '\0'; // End the string
    print("\n");

    if (strcmp(command_buffer, "cls") == 0) {
        draw_interface();
    } else if (strcmp(command_buffer, "ver") == 0) {
        print("Galaxy OS v1.0-alpha (Zinux Kernel)\n");
    } else if (strcmp(command_buffer, "whoami") == 0) {
        print("root\n");
    } else if (buffer_idx > 0) {
        print("Unknown Command: ");
        print(command_buffer);
        print("\n");
    }

    // Reset buffer for next command
    buffer_idx = 0;
    print("$ ");
}

void kernel_main() {
    draw_interface();

    while(1) {
        if (inb(0x64) & 0x01) {
            uint8_t scancode = inb(0x60);
            if (scancode < 128) {
                char letter = keyboard_map[scancode];
                
                if (scancode == 0x0E) { // Backspace scancode
                    backspace();
                } else if (letter == '\n') {
                    process_command();
                } else if (letter != 0 && buffer_idx < 63) {
                    command_buffer[buffer_idx++] = letter;
                    char s[2] = {letter, '\0'};
                    print(s);
                }
            }
        }
    }
}
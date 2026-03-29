#ifndef IOPUT_H
#define IOPUT_H

extern "C" {
    int write(int fd, const void* buf, unsigned int count);
    int read(int fd, void* buf, unsigned int count);
}

struct nl_t {};
constexpr nl_t nl{};

struct ll_t {};
constexpr ll_t ll{};

static char output_buffer[4096];
static int buffer_pos = 0;
static bool buffer_active = true;

static int float_precision = 6;
static int output_width = 0;
static char fill_char = '0';

inline void setdep(int precision) {
    if (precision >= 0 && precision <= 15) {
        float_precision = precision;
    }
}

inline void setlg(int width) {
    if (width >= 0) {
        output_width = width;
    }
}

inline void setlgc(char ch) {
    fill_char = ch;
}

static char input_buf[4096];
static int input_pos = 0;
static int input_len = 0;

inline int my_getchar() {
    if (input_pos < input_len) {
        return (unsigned char)input_buf[input_pos++];
    }
    input_len = read(0, input_buf, sizeof(input_buf));
    if (input_len <= 0) return -1;
    input_pos = 1;
    return (unsigned char)input_buf[0];
}

inline void my_ungetchar(int c) {
    if (c != -1 && input_pos > 0) {
        input_buf[--input_pos] = (char)c;
    }
}

inline void my_putchar(char c) {
    write(1, &c, 1);
    if (buffer_active && buffer_pos < sizeof(output_buffer) - 1) {
        output_buffer[buffer_pos++] = c;
    }
}

inline void my_puts(const char* s) {
    while (*s) my_putchar(*s++);
}

inline void my_putstr(const char* s) {
    if (!s) return;
    while (*s) my_putchar(*s++);
}

inline void clear() {
    for (int i = 0; i < buffer_pos; ++i) {
        my_putchar('\b');
        my_putchar(' ');
        my_putchar('\b');
    }
    buffer_pos = 0;
    output_buffer[0] = '\0';
}

inline void ll_simple() {
    for (int i = 0; i < buffer_pos; ++i) {
        my_putchar('\b');
    }
    my_putchar('\r');
    my_putchar('\033');
    my_putchar('[');
    my_putchar('A');
    buffer_pos = 0;
}

inline int utf8_width(const char* s) {
    int width = 0;
    while (*s) {
        unsigned char c = (unsigned char)*s;
        if (c < 0x80) {
            width += 1;
            s++;
        } else if ((c & 0xE0) == 0xC0) {
            width += 1;
            s += 2;
        } else if ((c & 0xF0) == 0xE0) {
            width += 2;
            s += 3;
        } else if ((c & 0xF8) == 0xF0) {
            width += 2;
            s += 4;
        } else {
            s++;
        }
    }
    return width;
}

inline int utf8_len(const char* s) {
    int len = 0;
    while (*s++) len++;
    return len;
}

inline void print_fill(int len) {
    for (int i = 0; i < len; ++i) {
        my_putchar(fill_char);
    }
}

template<typename T>
void print_int(T n) {
    char buf[32];
    int pos = 0;
    bool negative = false;
    
    if (n < 0) {
        negative = true;
        n = -n;
    }
    
    do {
        buf[pos++] = '0' + (n % 10);
        n /= 10;
    } while (n > 0);
    
    int num_len = pos;
    int total_len = num_len;
    if (negative) total_len++;
    
    if (output_width > 0 && output_width > total_len) {
        print_fill(output_width - total_len);
    }
    
    if (negative) {
        my_putchar('-');
    }
    
    for (int i = pos - 1; i >= 0; --i) {
        my_putchar(buf[i]);
    }
}

template<typename T>
void print_float(T f) {
    char buf[64];
    int pos = 0;
    bool negative = false;
    
    if (f < 0) {
        negative = true;
        f = -f;
    }
    
    long long int_part = (long long)f;
    f -= int_part;
    
    char int_buf[32];
    int int_pos = 0;
    do {
        int_buf[int_pos++] = '0' + (int_part % 10);
        int_part /= 10;
    } while (int_part > 0);
    
    for (int i = int_pos - 1; i >= 0; --i) {
        buf[pos++] = int_buf[i];
    }
    
    buf[pos++] = '.';
    
    long long fraction = 0;
    long long multiplier = 1;
    for (int i = 0; i < float_precision; ++i) {
        multiplier *= 10;
    }
    fraction = (long long)(f * multiplier + 0.5);
    
    char frac_buf[16];
    int frac_pos = 0;
    long long temp = fraction;
    for (int i = 0; i < float_precision; ++i) {
        if (temp > 0 || i == 0) {
            frac_buf[frac_pos++] = '0' + (temp % 10);
            temp /= 10;
        } else {
            frac_buf[frac_pos++] = '0';
        }
    }
    
    for (int i = frac_pos - 1; i >= 0; --i) {
        buf[pos++] = frac_buf[i];
    }
    
    int num_len = pos;
    int total_len = num_len;
    if (negative) total_len++;
    
    if (output_width > 0 && output_width > total_len) {
        print_fill(output_width - total_len);
    }
    
    if (negative) {
        my_putchar('-');
    }
    
    for (int i = 0; i < pos; ++i) {
        my_putchar(buf[i]);
    }
}

inline void print_one(int v) { print_int(v); }
inline void print_one(long v) { print_int(v); }
inline void print_one(long long v) { print_int(v); }
inline void print_one(unsigned int v) { print_int(v); }
inline void print_one(unsigned long v) { print_int(v); }
inline void print_one(unsigned long long v) { print_int(v); }
inline void print_one(float v) { print_float(v); }
inline void print_one(double v) { print_float(v); }

inline void print_one(char v) {
    if (output_width > 0 && output_width > 1) {
        print_fill(output_width - 1);
    }
    my_putchar(v);
}

inline void print_one(const char* v) {
    if (!v) {
        my_puts("(null)");
        return;
    }
    
    int disp_width = utf8_width(v);
    
    if (output_width > 0 && output_width > disp_width) {
        print_fill(output_width - disp_width);
    }
    
    my_putstr(v);
}

inline void print_one(char* v) { print_one((const char*)v); }

inline void print_one(nl_t) {
    my_putchar('\n');
    buffer_pos = 0;
}

inline void print_one(ll_t) {
    ll_simple();
}

template<typename T>
void read_int(T& n) {
    n = 0;
    int sign = 1;
    int c = my_getchar();
    while (c == ' ' || c == '\t' || c == '\n') c = my_getchar();
    if (c == '-') {
        sign = -1;
        c = my_getchar();
    } else if (c == '+') {
        c = my_getchar();
    }
    while (c >= '0' && c <= '9') {
        n = n * 10 + (c - '0');
        c = my_getchar();
    }
    n *= sign;
    if (c != -1) my_ungetchar(c);
}

template<typename T>
void read_unsigned(T& n) {
    n = 0;
    int c = my_getchar();
    while (c == ' ' || c == '\t' || c == '\n') c = my_getchar();
    while (c >= '0' && c <= '9') {
        n = n * 10 + (c - '0');
        c = my_getchar();
    }
    if (c != -1) my_ungetchar(c);
}

template<typename T>
void read_float(T& f) {
    f = 0.0;
    int sign = 1;
    int c = my_getchar();
    while (c == ' ' || c == '\t' || c == '\n') c = my_getchar();
    if (c == '-') {
        sign = -1;
        c = my_getchar();
    } else if (c == '+') {
        c = my_getchar();
    }
    long long integer = 0;
    while (c >= '0' && c <= '9') {
        integer = integer * 10 + (c - '0');
        c = my_getchar();
    }
    f = (T)integer;
    if (c == '.') {
        c = my_getchar();
        long long fraction = 0;
        long long divisor = 1;
        while (c >= '0' && c <= '9') {
            fraction = fraction * 10 + (c - '0');
            divisor *= 10;
            c = my_getchar();
        }
        if (divisor > 1) f += (T)fraction / (T)divisor;
    }
    f *= sign;
    if (c != -1) my_ungetchar(c);
}

inline void read_char(char& c) {
    int ch;
    do {
        ch = my_getchar();
    } while (ch == ' ' || ch == '\t' || ch == '\n');
    c = (char)ch;
}

inline void read_string(char* buf, int max_len) {
    int i = 0;
    int c = my_getchar();
    while (c == ' ' || c == '\t' || c == '\n') c = my_getchar();
    while (c != -1 && c != ' ' && c != '\t' && c != '\n' && i < max_len - 1) {
        buf[i++] = (char)c;
        c = my_getchar();
    }
    buf[i] = '\0';
    if (c != -1) my_ungetchar(c);
}

inline void read_one(int& v) { read_int(v); }
inline void read_one(long& v) { read_int(v); }
inline void read_one(long long& v) { read_int(v); }
inline void read_one(unsigned int& v) { read_unsigned(v); }
inline void read_one(unsigned long& v) { read_unsigned(v); }
inline void read_one(unsigned long long& v) { read_unsigned(v); }
inline void read_one(float& v) { read_float(v); }
inline void read_one(double& v) { read_float(v); }
inline void read_one(char& v) { read_char(v); }
inline void read_one(char* v) { read_string(v, 1024); }

inline void input() {}

template<typename T, typename... Args>
void input(T& first, Args&... args) {
    read_one(first);
    input(args...);
}

inline void output() {
    my_putchar('\n');
    buffer_pos = 0;
    output_width = 0;
    fill_char = '0';
}

template<typename T, typename... Args>
void output(const T& first, const Args&... args) {
    print_one(first);
    if (sizeof...(args) > 0) {
        my_putchar(' ');
        output(args...);
    } else {
        my_putchar('\n');
        buffer_pos = 0;
        output_width = 0;
        fill_char = '0';
    }
}

#endif

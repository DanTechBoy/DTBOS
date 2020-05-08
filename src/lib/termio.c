/**
 * SPDX-FileCopyrightText: 2020 Anuradha Weeraman <anuradha@weeraman.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <lib/termio.h>
#include <lib/string.h>
#include <sys/tty.h>

int putchar(int c)
{
        printc(c);
        return c;
}

/*
 * Like puts, without a newline at the end
 */
int prints(char *str)
{
        size_t i = 0;
        for (; str[i] != '\0'; i++) {
                printc(str[i]);
        }
        return i;
}

void printf(const char *fmt, ...)
{
        int           i;
        uint32_t      ul;
        uint64_t      ull;
        char          *str;
        char          print_buf[128];
        const char    *p;

        va_list arg;
        va_start(arg, fmt);

        for(p = fmt; *p != '\0'; p++) {
                if(*p != '%') {
                        putchar(*p);
                        continue;
                }

                switch(*++p) {
                        case 'c':
                        i = va_arg(arg, int);
                        putchar(i);
                        break;

                        case 's':
                        str = va_arg(arg, char*);
                        prints(str);
                        break;

                        case 'b':
                        ul = va_arg(arg, uint32_t);
                        prints(itoa((uint32_t) ul, print_buf, 2));
                        break;

                        case 'o':
                        ul = va_arg(arg, uint32_t);
                        prints(itoa((uint32_t) ul, print_buf, 8));
                        break;

                        case 'i':
                        case 'd':
                        i = va_arg(arg, int);
                        if (i < 0) {
                                i *= -1;
                                putchar('-');
                        }
                        prints(itoa((int) i, print_buf, 10));
                        break;

                        case 'u':
                        ul = va_arg(arg, uint32_t);
                        prints(itoa((uint32_t) ul, print_buf, 10));
                        break;

                        case 'x':
                        ul = va_arg(arg, uint32_t);
                        prints(itoa((uint32_t) ul, print_buf, 16));
                        break;

                        case 'X':
                        ull = va_arg(arg, uint64_t);
                        prints(itoa((uint64_t) ull, print_buf, 16));
                        break;

                        case '%':
                        prints("%");
                        break;
                }
        }

        va_end(arg);
}
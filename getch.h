// cross_platform_getch.h
#ifndef CROSS_PLATFORM_GETCH_H
#define CROSS_PLATFORM_GETCH_H

#ifdef _WIN32
    #include <conio.h>
    // Windows 下直接使用 _getch()
    #define getch() _getch()
#else
    // Linux / macOS 下的实现
    #include <termios.h>
    #include <unistd.h>
    #include <stdio.h>

    static inline int platform_getch(void) {
        struct termios oldt, newt;
        int ch;

        // 1. 获取当前终端属性
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;

        // 2. 修改属性：关闭标准模式(ICANON)和回显(ECHO)
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        // 3. 读取一个字符
        ch = getchar();

        // 4. 恢复终端原有属性
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        return ch;
    }
    #define getch() platform_getch()
#endif

#endif // CROSS_PLATFORM_GETCH_H
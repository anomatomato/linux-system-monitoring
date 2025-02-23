#ifndef COLORED_OUTPUT_H
#        define COLORED_OUTPUT_H

#        define RESET_ALL "\x1b[0m"

#        define COLOR_BLACK "\x1b[30m"
#        define COLOR_RED "\x1b[31m"
#        define COLOR_GREEN "\x1b[32m"
#        define COLOR_YELLOW "\x1b[33m"
#        define COLOR_BLUE "\x1b[34m"
#        define COLOR_MAGENTA "\x1b[35m"
#        define COLOR_CYAN "\x1b[36m"
#        define COLOR_WHITE "\x1b[37m"

#        define BACKGROUND_BLACK "\x1b[40m"
#        define BACKGROUND_RED "\x1b[41m"
#        define BACKGROUND_GREEN "\x1b[42m"
#        define BACKGROUND_YELLOW "\x1b[43m"
#        define BACKGROUND_BLUE "\x1b[44m"
#        define BACKGROUND_MAGENTA "\x1b[45m"
#        define BACKGROUND_CYAN "\x1b[46m"
#        define BACKGROUND_WHITE "\x1b[47m"

#        define STYLE_BOLD "\x1b[1m"
#        define STYLE_ITALIC "\x1b[3m"
#        define STYLE_UNDERLINE "\x1b[4m"

#endif

/* Example usage:

int main(int argc, const char **argv)
{
    puts("## Print color ##");
    printf("=> " COLOR_BLACK   "This text is BLACK!"   RESET_ALL "\n");
    printf("=> " COLOR_RED     "This text is RED!"     RESET_ALL "\n");
    printf("=> " COLOR_GREEN   "This text is GREEN!"   RESET_ALL "\n");
    printf("=> " COLOR_YELLOW  "This text is YELLOW!"  RESET_ALL "\n");
    printf("=> " COLOR_BLUE    "This text is BLUE!"    RESET_ALL "\n");
    printf("=> " COLOR_MAGENTA "This text is MAGENTA!" RESET_ALL "\n");
    printf("=> " COLOR_CYAN    "This text is CYAN!"    RESET_ALL "\n");
    printf("=> " COLOR_WHITE   "This text is WHITE!"   RESET_ALL "\n");

    puts("\n## Print style ##");
    printf("=> " STYLE_BOLD        "This text is BOLD!"      RESET_ALL "\n");
    printf("=> " STYLE_ITALIC      "This text is ITALIC!"    RESET_ALL "\n");
    printf("=> " STYLE_UNDERLINE   "This text is UNDERLINE!" RESET_ALL "\n");

    puts("\n## Print background ##");
    printf("=> " BACKGROUND_BLACK   "This BG is BLACK!"   RESET_ALL "\n");
    printf("=> " BACKGROUND_RED     "This BG is RED!"     RESET_ALL "\n");
    printf("=> " BACKGROUND_GREEN   "This BG is GREEN!"   RESET_ALL "\n");
    printf("=> " BACKGROUND_YELLOW  "This BG is YELLOW!"  RESET_ALL "\n");
    printf("=> " BACKGROUND_BLUE    "This BG is BLUE!"    RESET_ALL "\n");
    printf("=> " BACKGROUND_MAGENTA "This BG is MAGENTA!" RESET_ALL "\n");
    printf("=> " BACKGROUND_CYAN    "This BG is CYAN!"    RESET_ALL "\n");
    printf("=> " BACKGROUND_WHITE   "This BG is WHITE!"   RESET_ALL "\n");

    return 0;
}*/
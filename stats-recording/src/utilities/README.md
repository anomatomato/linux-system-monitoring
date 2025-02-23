# Utilities

- Library with functions and Macros, that are used by multiple Stats-components

## Line-protocol-parser

- Checks, if a message is in Influx-Line-Protocol format
> **Example**:
>   
> ```c
> #include line_protocol_parser.h
> 
> int main()
> {
>     const char *line = "measurement,tag=value field=\"Hello, world!\" 1570283407262541159";
>     int status = LP_check(line);
>     if (status != 0) {
>       printf("Not in Line-protocol-format!");
>     } else {
>       printf("In Line-protocol-format!");
>     }
> 
> }
> ```

## Colored Output

- Prints messages with color
> **Example**:
>  
> ```c
> 
>  #include colored_output.h
> int main(int argc, const char **argv)
> {
>     puts("## Print color ##");
>     printf("=> " COLOR_BLACK   "This text is BLACK!"   RESET_ALL "\n");
>     printf("=> " COLOR_RED     "This text is RED!"     RESET_ALL "\n");
>     printf("=> " COLOR_GREEN   "This text is GREEN!"   RESET_ALL "\n");
>     printf("=> " COLOR_YELLOW  "This text is YELLOW!"  RESET_ALL "\n");
>     printf("=> " COLOR_BLUE    "This text is BLUE!"    RESET_ALL "\n");
>     printf("=> " COLOR_MAGENTA "This text is MAGENTA!" RESET_ALL "\n");
>     printf("=> " COLOR_CYAN    "This text is CYAN!"    RESET_ALL "\n");
>     printf("=> " COLOR_WHITE   "This text is WHITE!"   RESET_ALL "\n");
> 
>     puts("\n## Print style ##");
>     printf("=> " STYLE_BOLD        "This text is BOLD!"      RESET_ALL "\n");
>     printf("=> " STYLE_ITALIC      "This text is ITALIC!"    RESET_ALL "\n");
>     printf("=> " STYLE_UNDERLINE   "This text is UNDERLINE!" RESET_ALL "\n");
> 
>     puts("\n## Print background ##");
>     printf("=> " BACKGROUND_BLACK   "This BG is BLACK!"   RESET_ALL "\n");
>     printf("=> " BACKGROUND_RED     "This BG is RED!"     RESET_ALL "\n");
>     printf("=> " BACKGROUND_GREEN   "This BG is GREEN!"   RESET_ALL "\n");
>     printf("=> " BACKGROUND_YELLOW  "This BG is YELLOW!"  RESET_ALL "\n");
>     printf("=> " BACKGROUND_BLUE    "This BG is BLUE!"    RESET_ALL "\n");
>     printf("=> " BACKGROUND_MAGENTA "This BG is MAGENTA!" RESET_ALL "\n");
>     printf("=> " BACKGROUND_CYAN    "This BG is CYAN!"    RESET_ALL "\n");
>     printf("=> " BACKGROUND_WHITE   "This BG is WHITE!"   RESET_ALL "\n");
> 
>     return 0;
> }
> ```
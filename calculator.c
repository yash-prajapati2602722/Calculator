#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_SIZE 256
#define PI 3.14159265358979323846
#define EULER 2.71828182845904523536

typedef struct {
    const char *input;
    const char *cursor;
    int degrees;
    int error;
} Parser;

static void skip_spaces(Parser *parser)
{
    while (isspace((unsigned char)*parser->cursor)) {
        parser->cursor++;
    }
}

static double parse_expression(Parser *parser);

static double parse_primary(Parser *parser)
{
    char *end;
    char name[16];
    size_t length;
    double value;

    skip_spaces(parser);
    if (*parser->cursor == '(') {
        parser->cursor++;
        value = parse_expression(parser);
        skip_spaces(parser);
        if (*parser->cursor != ')') {
            parser->error = 1;
            return 0.0;
        }
        parser->cursor++;
        return value;
    }

    errno = 0;
    value = strtod(parser->cursor, &end);
    if (end != parser->cursor) {
        if (errno == ERANGE) {
            parser->error = 1;
        }
        parser->cursor = end;
        return value;
    }

    if (!isalpha((unsigned char)*parser->cursor)) {
        parser->error = 1;
        return 0.0;
    }

    length = 0;
    while (isalpha((unsigned char)*parser->cursor)) {
        if (length + 1 < sizeof(name)) {
            name[length++] = (char)tolower((unsigned char)*parser->cursor);
        }
        parser->cursor++;
    }
    name[length] = '\0';

    if (strcmp(name, "pi") == 0) {
        return PI;
    }
    if (strcmp(name, "e") == 0) {
        return EULER;
    }

    skip_spaces(parser);
    if (*parser->cursor != '(') {
        parser->error = 1;
        return 0.0;
    }
    parser->cursor++;
    value = parse_expression(parser);
    skip_spaces(parser);
    if (*parser->cursor != ')') {
        parser->error = 1;
        return 0.0;
    }
    parser->cursor++;

    if (strcmp(name, "sin") == 0) {
        return sin(parser->degrees ? value * PI / 180.0 : value);
    }
    if (strcmp(name, "cos") == 0) {
        return cos(parser->degrees ? value * PI / 180.0 : value);
    }
    if (strcmp(name, "tan") == 0) {
        return tan(parser->degrees ? value * PI / 180.0 : value);
    }
    if (strcmp(name, "asin") == 0) {
        value = asin(value);
        return parser->degrees ? value * 180.0 / PI : value;
    }
    if (strcmp(name, "acos") == 0) {
        value = acos(value);
        return parser->degrees ? value * 180.0 / PI : value;
    }
    if (strcmp(name, "atan") == 0) {
        value = atan(value);
        return parser->degrees ? value * 180.0 / PI : value;
    }
    if (strcmp(name, "sqrt") == 0) {
        return sqrt(value);
    }
    if (strcmp(name, "log") == 0) {
        return log10(value);
    }
    if (strcmp(name, "ln") == 0) {
        return log(value);
    }
    if (strcmp(name, "abs") == 0) {
        return fabs(value);
    }

    parser->error = 1;
    return 0.0;
}

static double parse_power(Parser *parser)
{
    double left;

    skip_spaces(parser);
    if (*parser->cursor == '+') {
        parser->cursor++;
        return parse_power(parser);
    }
    if (*parser->cursor == '-') {
        parser->cursor++;
        return -parse_power(parser);
    }

    left = parse_primary(parser);
    skip_spaces(parser);
    if (*parser->cursor == '^') {
        parser->cursor++;
        left = pow(left, parse_power(parser));
    }
    return left;
}

static double parse_term(Parser *parser)
{
    double value = parse_power(parser);

    for (;;) {
        double right;
        skip_spaces(parser);
        if (*parser->cursor != '*' && *parser->cursor != '/') {
            return value;
        }
        if (*parser->cursor == '*') {
            parser->cursor++;
            value *= parse_power(parser);
        } else {
            parser->cursor++;
            right = parse_power(parser);
            if (right == 0.0) {
                parser->error = 1;
                return 0.0;
            }
            value /= right;
        }
    }
}

static double parse_expression(Parser *parser)
{
    double value = parse_term(parser);

    for (;;) {
        skip_spaces(parser);
        if (*parser->cursor == '+') {
            parser->cursor++;
            value += parse_term(parser);
        } else if (*parser->cursor == '-') {
            parser->cursor++;
            value -= parse_term(parser);
        } else {
            return value;
        }
    }
}

static int calculate(const char *input, int degrees, double *result)
{
    Parser parser = {input, input, degrees, 0};

    *result = parse_expression(&parser);
    skip_spaces(&parser);
    if (parser.error || parser.cursor == parser.input || *parser.cursor != '\0') {
        return 0;
    }
    return isfinite(*result);
}

static void print_help(void)
{
    puts("Enter an expression, or use a command:");
    puts("  mode deg       use degrees for trigonometry");
    puts("  mode rad       use radians for trigonometry");
    puts("  help           show this help");
    puts("  quit           exit the calculator");
    puts("\nOperators: +  -  *  /  ^  and parentheses");
    puts("Functions: sin cos tan asin acos atan sqrt log ln abs");
    puts("Constants: pi e");
}

int main(void)
{
    char input[INPUT_SIZE];
    int degrees = 1;

    puts("FX-991CW-style C calculator");
    puts("Type help for commands. Trigonometry starts in degree mode.");
    for (;;) {
        char command[INPUT_SIZE];
        double result;

        printf("[%s] > ", degrees ? "DEG" : "RAD");
        if (!fgets(input, sizeof(input), stdin)) {
            putchar('\n');
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        if (sscanf(input, "%255s", command) != 1) {
            continue;
        }
        if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
            break;
        }
        if (strcmp(command, "help") == 0) {
            print_help();
            continue;
        }
        if (strncmp(input, "mode", 4) == 0) {
            if (strstr(input, "deg") != NULL) {
                degrees = 1;
                puts("Degree mode enabled.");
            } else if (strstr(input, "rad") != NULL) {
                degrees = 0;
                puts("Radian mode enabled.");
            } else {
                puts("Use: mode deg or mode rad");
            }
            continue;
        }

        if (calculate(input, degrees, &result)) {
            printf("= %.12g\n", result);
        } else {
            puts("Error: invalid expression, division by zero, or undefined result.");
        }
    }
    return 0;
}
#pragma once

# define RESET          "\033[0m"
# define RED            "\033[31m"
# define GREEN          "\033[32m"
# define MAGENTA        "\033[35m"
# define YELLOW         "\033[33m"
# define BLUE           "\033[34m"
# define CYAN           "\033[36m"
# define WHITE          "\033[37m"
# define B_RED          "\033[1m\033[31m"
# define B_GREEN        "\033[1m\033[32m"
# define B_YELLOW       "\033[1m\033[33m"
# define B_BLUE         "\033[1m\033[34m"
# define B_MAGENTA      "\033[1m\033[35m"
# define B_CYAN         "\033[1m\033[36m"
# define B_WHITE        "\033[1m\033[37m"

# define PRINT_COLOR(color, text) (std::cout << color << text << RESET << std::endl)
# define PRINT_ERROR(color, text) (std::cerr << color << text << RESET << std::endl)

# define DEBUG          1
# define DEBUG_ON       1
# define DEBUG_OFF      0

# define EXIT_FAILIURE  1
# define EXIT_SUCCESS   0

# define TRUE   1
# define FALSE  0

# define DEFAULT_CONSTRUCTOR		"[Default constructor called]"
# define DEFAULT_DESTRUCTOR			"[Default destructor called]"
# define PARAMETER_CONSTRUCTOR		"[Parameter constructor called]"
# define COPY_CONSTRUCTOR			"[Copy constructor called]"
# define ASSIGNMENT_OPERATOR		"[Assignment operator called]"

# define ERR_MSG_NUMBER_OF_ARGS		"ERROR: Invalid number of arguments"
# define ERR_MSG_OPEN_FILE			"ERROR: Could not open fd!"

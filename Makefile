# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: exam <exam@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/26 01:36:00 by exam              #+#    #+#              #
#    Updated: 2025/10/26 01:36:00 by exam             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = gcc
CFLAGS = -Wall -Wextra -Werror

# Exercise 00
EX00_SRC = ex00/aff_a.c
EX00_BIN = ex00/aff_a

# Exercise 01
EX01_SRC = ex01/ft_strlen.c
EX01_BIN = ex01/ft_strlen

# Exercise 02
EX02_SRC = ex02/ft_swap.c
EX02_BIN = ex02/ft_swap

all: ex00 ex01 ex02

ex00:
	@echo "Building exercise 00..."
	@$(CC) $(CFLAGS) $(EX00_SRC) -o $(EX00_BIN)
	@echo "✓ ex00 compiled successfully"

ex01:
	@echo "Building exercise 01..."
	@echo "Note: ex01 needs a main function for testing"

ex02:
	@echo "Building exercise 02..."
	@echo "Note: ex02 needs a main function for testing"

clean:
	@rm -f ex00/*.o ex01/*.o ex02/*.o
	@echo "✓ Object files cleaned"

fclean: clean
	@rm -f $(EX00_BIN) $(EX01_BIN) $(EX02_BIN)
	@echo "✓ Binaries cleaned"

re: fclean all

test: all
	@echo "Running tests..."
	@echo "\n=== Testing ex00 ==="
	@./$(EX00_BIN) "abc" || true
	@./$(EX00_BIN) "mangue" "juice" || true
	@./$(EX00_BIN) || true

.PHONY: all ex00 ex01 ex02 clean fclean re test

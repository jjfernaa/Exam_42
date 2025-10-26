# Exam 42 School

This repository contains practice exam exercises for students at 42 School.

## 📚 About

42 School exams are designed to test your understanding of fundamental programming concepts in C. This repository provides a structured environment for practicing common exam exercises.

## 🏗️ Structure

```
Exam_42/
├── subject.en.txt          # Main exam guidelines and rules
├── Makefile                # Build automation
├── ex00/                   # Exercise 00: aff_a
│   ├── subject.txt
│   └── aff_a.c
├── ex01/                   # Exercise 01: ft_strlen
│   ├── subject.txt
│   └── ft_strlen.c
└── ex02/                   # Exercise 02: ft_swap
    ├── subject.txt
    └── ft_swap.c
```

## 🚀 Getting Started

### Prerequisites
- GCC compiler
- Make

### Compilation

To compile all exercises:
```bash
make
```

To compile a specific exercise:
```bash
make ex00
make ex01
make ex02
```

To run tests:
```bash
make test
```

To clean up:
```bash
make clean    # Remove object files
make fclean   # Remove object files and binaries
make re       # Rebuild everything
```

## 📝 Exercises

### Exercise 00: aff_a
**Difficulty**: Easy  
**Description**: Display the character 'a' from a string argument.

### Exercise 01: ft_strlen
**Difficulty**: Easy  
**Description**: Implement a function that returns the length of a string.

### Exercise 02: ft_swap
**Difficulty**: Easy  
**Description**: Implement a function that swaps two integers.

## 📖 Exam Rules

- All code must compile with `-Wall -Wextra -Werror` flags
- No memory leaks allowed
- Functions should not crash (no segfaults, bus errors, etc.)
- Follow the 42 Norm coding standards
- Each exercise must be in its own directory

## 🎯 Learning Objectives

- Understanding pointers and memory management
- String manipulation in C
- Working with command-line arguments
- Writing clean, norminette-compliant code

## 📄 License

This is an educational repository for 42 School students.

## ✨ Tips

1. Read the subject carefully before coding
2. Test your code with various edge cases
3. Check for memory leaks with valgrind
4. Make sure your code follows the Norm
5. Don't forget to free allocated memory

Good luck with your exam! 🍀
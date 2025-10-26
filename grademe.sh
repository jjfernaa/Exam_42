#!/bin/bash
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    grademe.sh                                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: exam <exam@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/26 01:36:00 by exam              #+#    #+#              #
#    Updated: 2025/10/26 01:36:00 by exam             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Score tracking
TOTAL_SCORE=0
MAX_SCORE=0

echo -e "${BLUE}╔════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║     42 School Exam Auto-Grader        ║${NC}"
echo -e "${BLUE}╔════════════════════════════════════════╗${NC}"
echo ""

# Function to test ex00
test_ex00() {
    echo -e "\n${YELLOW}Testing Exercise 00: aff_a${NC}"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    if [ ! -f "ex00/aff_a.c" ]; then
        echo -e "${RED}✗ aff_a.c not found${NC}"
        return
    fi
    
    gcc -Wall -Wextra -Werror ex00/aff_a.c -o ex00/aff_a 2>/dev/null
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}✗ Compilation failed${NC}"
        return
    fi
    
    # Test cases
    result=$(./ex00/aff_a "abc")
    if [ "$result" = "a" ]; then
        echo -e "${GREEN}✓ Test 1 passed: aff_a \"abc\"${NC}"
        TOTAL_SCORE=$((TOTAL_SCORE + 10))
    else
        echo -e "${RED}✗ Test 1 failed${NC}"
    fi
    
    result=$(./ex00/aff_a "mangue" "juice")
    if [ "$result" = "" ]; then
        echo -e "${GREEN}✓ Test 2 passed: aff_a with 2 args${NC}"
        TOTAL_SCORE=$((TOTAL_SCORE + 5))
    else
        echo -e "${RED}✗ Test 2 failed${NC}"
    fi
    
    MAX_SCORE=$((MAX_SCORE + 15))
}

# Function to test ex01
test_ex01() {
    echo -e "\n${YELLOW}Testing Exercise 01: ft_strlen${NC}"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    if [ ! -f "ex01/ft_strlen.c" ]; then
        echo -e "${RED}✗ ft_strlen.c not found${NC}"
        return
    fi
    
    # Create test main
    cat > /tmp/test_strlen.c << 'EOF'
#include <stdio.h>
int ft_strlen(char *str);

int main(void)
{
    if (ft_strlen("Hello") == 5 &&
        ft_strlen("") == 0 &&
        ft_strlen("42 School") == 9)
    {
        printf("OK\n");
        return 0;
    }
    return 1;
}
EOF
    
    gcc -Wall -Wextra -Werror ex01/ft_strlen.c /tmp/test_strlen.c -o /tmp/test_strlen 2>/dev/null
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}✗ Compilation failed${NC}"
        return
    fi
    
    result=$(/tmp/test_strlen)
    if [ "$result" = "OK" ]; then
        echo -e "${GREEN}✓ All tests passed${NC}"
        TOTAL_SCORE=$((TOTAL_SCORE + 15))
    else
        echo -e "${RED}✗ Tests failed${NC}"
    fi
    
    MAX_SCORE=$((MAX_SCORE + 15))
}

# Function to test ex02
test_ex02() {
    echo -e "\n${YELLOW}Testing Exercise 02: ft_swap${NC}"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    if [ ! -f "ex02/ft_swap.c" ]; then
        echo -e "${RED}✗ ft_swap.c not found${NC}"
        return
    fi
    
    # Create test main
    cat > /tmp/test_swap.c << 'EOF'
#include <stdio.h>
void ft_swap(int *a, int *b);

int main(void)
{
    int x = 42;
    int y = 24;
    
    ft_swap(&x, &y);
    
    if (x == 24 && y == 42)
    {
        printf("OK\n");
        return 0;
    }
    return 1;
}
EOF
    
    gcc -Wall -Wextra -Werror ex02/ft_swap.c /tmp/test_swap.c -o /tmp/test_swap 2>/dev/null
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}✗ Compilation failed${NC}"
        return
    fi
    
    result=$(/tmp/test_swap)
    if [ "$result" = "OK" ]; then
        echo -e "${GREEN}✓ All tests passed${NC}"
        TOTAL_SCORE=$((TOTAL_SCORE + 20))
    else
        echo -e "${RED}✗ Tests failed${NC}"
    fi
    
    MAX_SCORE=$((MAX_SCORE + 20))
}

# Run all tests
test_ex00
test_ex01
test_ex02

# Calculate percentage
PERCENTAGE=$((TOTAL_SCORE * 100 / MAX_SCORE))

# Display final score
echo ""
echo -e "${BLUE}╔════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║            Final Score                 ║${NC}"
echo -e "${BLUE}╠════════════════════════════════════════╣${NC}"

if [ $PERCENTAGE -ge 80 ]; then
    echo -e "${BLUE}║${GREEN}  Score: $TOTAL_SCORE/$MAX_SCORE ($PERCENTAGE%)                  ${BLUE}║${NC}"
elif [ $PERCENTAGE -ge 50 ]; then
    echo -e "${BLUE}║${YELLOW}  Score: $TOTAL_SCORE/$MAX_SCORE ($PERCENTAGE%)                  ${BLUE}║${NC}"
else
    echo -e "${BLUE}║${RED}  Score: $TOTAL_SCORE/$MAX_SCORE ($PERCENTAGE%)                   ${BLUE}║${NC}"
fi

echo -e "${BLUE}╚════════════════════════════════════════╝${NC}"
echo ""

# Cleanup
rm -f ex00/aff_a /tmp/test_strlen /tmp/test_swap /tmp/test_*.c

exit 0

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct node {
    enum {
        ADD,
        MULTI,
        VAL
    }   type;
    int val;
    struct node *l;
    struct node *r;
}   node;

node    *parse_primary(char **s);
node	*parse_mult(char **s);
node	*parse_add(char **s);
node	*parse_expr(char **s);

node    *new_node(node n)
{
    node *ret = calloc(1, sizeof(node));
    if (!ret)
        return (NULL);
    *ret = n;
    return (ret);
}

void    destroy_tree(node *n)
{
    if (!n)
        return ;
    if (n->type != VAL)
    {
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    free(n);
}

void    unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
}

int accept(char **s, char c)
{
    if (**s == c)
    {
        (*s)++;
        return (1);
    }
    return (0);
}

int expect(char **s, char c)
{
    if (accept(s, c))
        return (1);
    unexpected(**s);
    return (0);
}
node	*parse_primary(char **s)
{
	if(**s == '(')
	{
		(*s)++;
		node *result = parse_add(s);
		if(!result || **s != ')')
		{
			destroy_tree(result);
			unexpected(**s);
			return (NULL);
		}
		(*s)++;
		return (result);
	}
	if (isdigit(**s))
	{
		node	temp;
		temp.type = VAL;
		temp.val = **s - '0';
		temp.l = NULL;
		temp.r = NULL;
		(*s)++;
		return (new_node(temp));
	}
	unexpected(**s);
	return (NULL);
}
node	*parse_mult(char **s)
{
	node	*left = parse_primary(s);
	if (!left)
		return(NULL);
	while (**s == '*')
	{
		(*s)++;
		node	*right = parse_primary(s);
		if(!right)
		{
			destroy_tree(left);
			return(NULL);
		}
		node	temp;
		temp.type = MULTI;
		temp.l = left;
		temp.r = right;
		temp.val = 0;
		left = new_node(temp);
		if(!left)
		{
			destroy_tree(right);
			return(NULL);
		}
	}
	return (left);
}
node	*parse_add(char **s)
{
	node	*left = parse_mult(s);
	if (!left)
		return(NULL);
	while(**s == '+')
	{
		(*s)++;
		node	*right = parse_mult(s);
		if(!right)
		{
			destroy_tree(left);
			return(NULL);
		}
		node	temp;
		temp.type = ADD;
		temp.l = left;
		temp.r = right;
		temp.val = 0;
		left = new_node(temp);
		if (!left)
		{
			destroy_tree(right);
			return (NULL);
		}
	}
	return(left);
}

node    *parse_expr(char **s)
{
    node	*ret = parse_add(s);
	if (!ret)
		return(NULL);
    if (**s) 
    {
        destroy_tree(ret);
		unexpected(**s);
        return (NULL);
    }
    return (ret);
}

int eval_tree(node *tree)
{
    switch (tree->type)
    {
        case ADD:
            return (eval_tree(tree->l) + eval_tree(tree->r));
        case MULTI:
            return (eval_tree(tree->l) * eval_tree(tree->r));
        case VAL:
            return (tree->val);
    }
	return (0);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
	char	*input = argv[1];
    node *tree = parse_expr(&input);
    if (!tree)
        return (1);
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
	return (0);
}

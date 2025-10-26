/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juan-jof <juan-jof@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 01:24:09 by juan-jof          #+#    #+#             */
/*   Updated: 2025/06/13 00:20:16 by juan-jof         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 42
#endif

int	ft_strlen(const char *str)
{
	int i;
	
	i = 0;
	while (str[i])
		i++;
	return (i);
}

char	*ft_strchr(char *str, char c)
{
	while (*str)
	{
		if (*str == c)
			return (str);
		str++;
	}
	return NULL;
}

void ft_strcpy(char *dst, char *src)
{
	while (*src)
		*dst++ = *src++;
	*dst = '\0';
}

char	*ft_strdup(char *src)
{
	char *dst;

	dst = malloc((ft_strlen(src) + 1));
	if (!dst)
		return (NULL);
	ft_strcpy(dst, src);
	return(dst);
}

char	*ft_strjoin(char *s1, char *s2)
{
	char	*join;
	
	if (!s1 || !s2)
		return (NULL);
	join = malloc((ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!join)
		return (NULL);
	ft_strcpy(join, s1);
	ft_strcpy((join + ft_strlen(s1)), s2);
	free(s1);
	return (join);
}

char	*get_next_line(int fd)
{
	static char buffer[BUFFER_SIZE +1];
	char		*line;
	char		*newline;
	int			byte;
	int			to_copy;
	
	line = ft_strdup(buffer);
	while (!(newline = ft_strchr(line, '\n')) && (byte = read(fd, buffer, BUFFER_SIZE)))
	{
		buffer[byte] = '\0';
		line = ft_strjoin(line, buffer);
	}
	if (ft_strlen(line) == 0)
		return (free(line), NULL);
	if (newline != NULL)
	{
		to_copy = newline - line + 1;
		ft_strcpy(buffer, newline+ 1);
	}
	else
	{
		to_copy = ft_strlen(line);
		buffer[0] = '\0';
	}
	line[to_copy] = '\0';
	return (line);
}

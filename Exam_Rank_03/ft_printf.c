/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juan-jof <juan-jof@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 03:13:45 by juan-jof          #+#    #+#             */
/*   Updated: 2025/06/05 02:10:54 by juan-jof         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdarg.h>

int	ft_putchar(char c)
{
	return write(1, &c, 1);
}

int	ft_putstr(char *str)
{
	int i;

	i = 0;
	if (!str)
		return write(1, "(null)", 6);
	while (str[i])
		write(1, &str[i++], 1);
	return i;
}

int	ft_putnbr(int n)
{
	int	len;

	len = 0;
	if (n == -2147483648)
		return write(1, "-2147483648", 11);
	if (n < 0)
	{
		len += ft_putchar('-');
		n = -n;
	}
	if (n >= 10)
		len += ft_putnbr(n /10);
	len += ft_putchar((n % 10) + '0');
	return len;
}

int	ft_puthex(unsigned int n)
{
	char	*hex;
	int		len;

	hex = "0123456789abcdef";
	len = 0;
	if (n >= 16)
		len += ft_puthex(n / 16);
	len += ft_putchar(hex[n % 16]);
	return len;
}

int	ft_printf(const char *format, ...)
{
	va_list args;
	int		i;
	int		len;
	
	i = 0;
	len = 0;
	va_start(args, format);
	while (format[i])
	{
		if (format[i] == '%' && format[i + 1])
		{
			i++;
			if (format[i] == 's')
				len += ft_putstr(va_arg(args, char *));
			else if (format[i] == 'd')
				len += ft_putnbr(va_arg(args, int));
			else if (format[i] == 'x')
				len += ft_puthex(va_arg(args, unsigned int));
		}
		else
			len += ft_putchar(format[i]);
		i++;
	}
	va_end(args);
	return len;
}

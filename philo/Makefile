SRCS		=	basic_tools.c \
				init.c \
				death.c \
				eat.c \
				other_actions.c \
				free_and_routine.c \
				main.c \
				handle_forks.c

OBJS		= ${SRCS:.c=.o}

NAME		= philo

CC			= cc

CFLAGS		= -Wall -Wextra -Werror -I.

RM			= rm -f

%.o: %.c
	${CC} ${CFLAGS} -c $< -o ${<:.c=.o}

${NAME}:		 ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -pthread -o ${NAME}

all:			${NAME}

clean:
	${RM} ${OBJS}

fclean:			clean
	${RM} ${NAME}

re:				fclean all

re_bonus:		fclean_bonus bonus

.PHONY:			all clean fclean re

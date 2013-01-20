PROG=	ttyconsole
SRCS=	ttyconsole.c

NOMAN=	yes

LDADD+=	-lutil
DPADD+=	${LIBUTIL}

.include <bsd.prog.mk>

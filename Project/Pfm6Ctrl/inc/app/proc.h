typedef	void *func(void *);
typedef	void *arg;

typedef struct {
	func 	*f;
	arg 	*arg;
	int		t,dt,to;
	char	*name;
} _proc;

void	_proc_add(func *,arg *,char *, int);
_proc		*_proc_find(func *,arg *);
void	_proc_remove(func *,arg *);
void	_proc_list(void);

void	_wait(int,void (*)(void));

typedef	void *func(void *);
typedef	void *arg;

typedef struct {
	func 	*f;
	arg 	*arg;
	int		t,dt,to;
	char	*name;
} app;

void	App_Add(func *,arg *,char *, int);
app		*App_Find(func *,arg *);
void	App_Remove(func *,arg *);
void	App_List(void);

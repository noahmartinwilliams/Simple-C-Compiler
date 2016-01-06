struct a_t {
	int i;
	char c;
} ;

extern struct a_t* malloc(size_t s);
extern void free(struct a_t *s);

int main()
{
	struct a_t *b=malloc(sizeof(struct a_t));
	b->i=1;
	if (b->i==1) {
		b->c='a';
		if (b->c=='a') {
			free(b);
			return 0;
		} else {
			free(b);
			return 2;
		}
	} else {
		free(b);
		return 1;
	}
}

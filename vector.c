#include "ikacc.h"

Vector	*vec_new(void)
{
	Vector	*new;

	new = (Vector *)calloc(1, sizeof(Vector));
	new->data = (void **)calloc(10, sizeof(void *));
	new->capacity = 10;
	new->len = 0;
	return new;
}

void	vec_push(Vector **this, void *data)
{
	// DBG();
	if (*this == NULL)
	{
		*this = vec_new();
	}
	if ((*this)->len == (*this)->capacity - 1)
	{
		(*this)->data = realloc((*this)->data, sizeof(void *) * ((*this)->capacity + 10));
		(*this)->capacity += 10;
	}
	(*this)->data[(*this)->len] = data;
	(*this)->len++;
}

void	vec_dump(Vector *this)
{
	for (int i = 0; i < this->len; i++) {
		fprintf(stderr, "vec[%d] : %p\n", i, this->data);
	}
}

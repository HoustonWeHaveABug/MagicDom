#include <stdio.h>
#include <stdlib.h>

#define MAGICDOM_ORDER_MAX 256

struct dom_s {
	unsigned long *values;
	unsigned long sum;
	int used;
};
typedef struct dom_s dom_t;

struct sum_s {
	unsigned long len;
	unsigned long value;
};
typedef struct sum_s sum_t;

void init_sum(sum_t *);
void magic_square(unsigned long, unsigned long, unsigned long);
void add_row_dom(unsigned long, unsigned long, unsigned long, unsigned long, dom_t *);
void add_col_dom(unsigned long, unsigned long, unsigned long, unsigned long, dom_t *);
void set_used(dom_t *, int);
int add_sum(sum_t *, unsigned long, unsigned long);
void sub_sum(sum_t *, unsigned long, unsigned long);
void free_values(unsigned long);
void free_doms(unsigned long);

int *value_used;
unsigned long order, value_max, dom_len, doms_n, **values, magic, nodes, solutions;
dom_t *doms;
sum_t *row_sums, *col_sums, diag1_sum, diag2_sum;

int main(void) {
unsigned long i, j;
	if (scanf("%lu", &order) != 1 || order < 3 || order > MAGICDOM_ORDER_MAX) {
		fprintf(stderr, "Invalid order\n");
		return EXIT_FAILURE;
	}
	value_max = order*order;
	if (scanf("%lu", &dom_len) != 1 || dom_len < 1 || value_max%dom_len) {
		fprintf(stderr, "Invalid domino length\n");
		return EXIT_FAILURE;
	}
	doms_n = value_max/dom_len;
	doms = malloc(sizeof(dom_t)*doms_n);
	if (!doms) {
		fprintf(stderr, "Could not allocate doms\n");
		return EXIT_FAILURE;
	}
	for (i = 0; i < doms_n; i++) {
		doms[i].values = malloc(sizeof(unsigned long)*dom_len);
		if (!doms[i].values) {
			fprintf(stderr, "Could not allocate doms[%lu].values\n", i);
			free_doms(i);
			return EXIT_FAILURE;
		}
	}
	value_used = calloc(value_max, sizeof(int));
	if (!value_used) {
		fprintf(stderr, "Could not allocate value_used\n");
		free_doms(doms_n);
		return EXIT_FAILURE;
	}
	for (i = 0; i < doms_n; i++) {
		doms[i].sum = 0;
		for (j = 0; j < dom_len; j++) {
			if (scanf("%lu", &doms[i].values[j]) != 1 || doms[i].values[j] < 1 || doms[i].values[j] > value_max || value_used[doms[i].values[j]-1]) {
				fprintf(stderr, "Invalid value in domino %lu position %lu\n", i+1, j+1);
				free(value_used);
				free_doms(doms_n);
				return EXIT_FAILURE;
			}
			doms[i].sum += doms[i].values[j];
			value_used[doms[i].values[j]-1] = 1;
		}
		doms[i].used = 0;
	}
	for (i = 0; i < value_max; i++) {
		value_used[i] = 0;
	}
	values = malloc(sizeof(unsigned long *)*order);
	if (!values) {
		fprintf(stderr, "Could not allocate values\n");
		free(value_used);
		free_doms(doms_n);
		return EXIT_FAILURE;
	}
	for (i = 0; i < order; i++) {
		values[i] = calloc(order, sizeof(unsigned long));
		if (!values[i]) {
			fprintf(stderr, "Could not allocate values[%lu]\n", i);
			free_values(i);
			free(value_used);
			free_doms(doms_n);
			return EXIT_FAILURE;
		}
	}
	row_sums = malloc(sizeof(sum_t)*order);
	if (!row_sums) {
		fprintf(stderr, "Could not allocate row_sums\n");
		free_values(order);
		free(value_used);
		free_doms(doms_n);
		return EXIT_FAILURE;
	}
	col_sums = malloc(sizeof(sum_t)*order);
	if (!col_sums) {
		fprintf(stderr, "Could not allocate col_sums\n");
		free(row_sums);
		free_values(order);
		free(value_used);
		free_doms(doms_n);
		return EXIT_FAILURE;
	}
	for (i = 0; i < order; i++) {
		init_sum(row_sums+i);
	}
	for (i = 0; i < order; i++) {
		init_sum(col_sums+i);
	}
	init_sum(&diag1_sum);
	init_sum(&diag2_sum);
	magic = order*(value_max+1)/2;
	nodes = 0;
	solutions = 0;
	magic_square(0UL, 0UL, 0UL);
	printf("Nodes %lu\nSolutions %lu\n", nodes, solutions);
	free(col_sums);
	free(row_sums);
	free_values(order);
	free(value_used);
	free_doms(doms_n);
	return EXIT_SUCCESS;
}

void init_sum(sum_t *sum) {
	sum->len = 0;
	sum->value = 0;
}

void magic_square(unsigned long dom_n, unsigned long row, unsigned long col) {
unsigned long last, i, j;
	nodes++;
	if (dom_n < doms_n) {
		while (col < order && values[row][col]) {
			col++;
		}
		while (col == order) {
			for (col = 0; col < order && values[row][col]; col++);
			if (col == order) {
				row++;
			}
		}
		last = col+dom_len;
		for (i = col; i < last && i < order && !values[row][i]; i++);
		if (i == last) {
			for (i = 0; i < doms_n; i++) {
				add_row_dom(dom_n, row, col, last, doms+i);
			}
		}
		if (dom_len > 1) {
			last = row+dom_len;
			for (i = row; i < last && i < order && !values[i][col]; i++);
			if (i == last) {
				for (i = 0; i < doms_n; i++) {
					add_col_dom(dom_n, row, col, last, doms+i);
				}
			}
		}
	}
	else {
		solutions++;
		if (solutions == 1) {
			for (i = 0; i < order; i++) {
				printf("%lu", values[i][0]);
				for (j = 1; j < order; j++) {
					printf(" %lu", values[i][j]);
				}
				puts("");
			}
		}
	}
}

void add_row_dom(unsigned long dom_n, unsigned long row, unsigned long col, unsigned long last, dom_t *dom) {
int diag1_done, diag2_done;
unsigned long ic, id, jc, jd;
	if (!dom->used) {
		set_used(dom, 1);
		if (add_sum(row_sums+row, dom_len, dom->sum)) {
			for (ic = col, id = 0; ic < last && add_sum(col_sums+ic, 1UL, dom->values[id]); ic++, id++);
			if (ic == last) {
				diag1_done = 2;
				diag2_done = 2;
				for (jc = col, jd = 0; jc < last && diag1_done && diag2_done; jc++, jd++) {
					if (jc == row) {
						diag1_done = add_sum(&diag1_sum, 1UL, dom->values[jd]);
					}
					if (row+jc == order-1) {
						diag2_done = add_sum(&diag2_sum, 1UL, dom->values[jd]);
					}
				}
				if (diag1_done && diag2_done) {
					for (jc = col, jd = 0; jc < last; jc++, jd++) {
						values[row][jc] = dom->values[jd];
					}
					magic_square(dom_n+1, row, col);
					for (jc = col; jc < last; jc++) {
						values[row][jc] = 0;
					}
				}
				for (jc = col, jd = 0; jc < last; jc++, jd++) {
					if (row+jc == order-1 && diag2_done < 2) {
						sub_sum(&diag2_sum, 1UL, dom->values[jd]);
					}
					if (jc == row && diag1_done < 2) {
						sub_sum(&diag1_sum, 1UL, dom->values[jd]);
					}
				}
				ic--;
				id--;
			}
			while (ic > col) {
				sub_sum(col_sums+ic, 1UL, dom->values[id]);
				ic--;
				id--;
			}
			sub_sum(col_sums+ic, 1UL, dom->values[0]);
			if (dom_len > 1) {
				for (ic = col, id = dom_len; ic < last && add_sum(col_sums+ic, 1UL, dom->values[id-1]); ic++, id--);
				if (ic == last) {
					diag1_done = 2;
					diag2_done = 2;
					for (jc = col, jd = dom_len; jc < last && diag1_done && diag2_done; jc++, jd--) {
						if (jc == row) {
							diag1_done = add_sum(&diag1_sum, 1UL, dom->values[jd-1]);
						}
						if (row+jc == order-1) {
							diag2_done = add_sum(&diag2_sum, 1UL, dom->values[jd-1]);
						}
					}
					if (diag1_done && diag2_done) {
						for (jc = col, jd = dom_len; jc < last; jc++, jd--) {
							values[row][jc] = dom->values[jd-1];
						}
						magic_square(dom_n+1, row, col);
						for (jc = col; jc < last; jc++) {
							values[row][jc] = 0;
						}
					}
					for (jc = col, jd = dom_len; jc < last; jc++, jd--) {
						if (row+jc == order-1 && diag2_done < 2) {
							sub_sum(&diag2_sum, 1UL, dom->values[jd-1]);
						}
						if (jc == row && diag1_done < 2) {
							sub_sum(&diag1_sum, 1UL, dom->values[jd-1]);
						}
					}
					ic--;
					id++;
				}
				while (ic > col) {
					sub_sum(col_sums+ic, 1UL, dom->values[id-1]);
					ic--;
					id++;
				}
				sub_sum(col_sums+ic, 1UL, dom->values[dom_len-1]);
			}
		}
		sub_sum(row_sums+row, dom_len, dom->sum);
		set_used(dom, 0);
	}
}

void add_col_dom(unsigned long dom_n, unsigned long row, unsigned long col, unsigned long last, dom_t *dom) {
int diag1_done, diag2_done;
unsigned long ir, id, jr, jd;
	if (!dom->used) {
		set_used(dom, 1);
		if (add_sum(col_sums+col, dom_len, dom->sum)) {
			for (ir = row, id = 0; ir < last && add_sum(row_sums+ir, 1UL, dom->values[id]); ir++, id++);
			if (ir == last) {
				diag1_done = 2;
				diag2_done = 2;
				for (jr = row, jd = 0; jr < last && diag1_done && diag2_done; jr++, jd++) {
					if (jr == col) {
						diag1_done = add_sum(&diag1_sum, 1UL, dom->values[jd]);
					}
					if (col+jr == order-1) {
						diag2_done = add_sum(&diag2_sum, 1UL, dom->values[jd]);
					}
				}
				if (diag1_done && diag2_done) {
					for (jr = row, jd = 0; jr < last; jr++, jd++) {
						values[jr][col] = dom->values[jd];
					}
					magic_square(dom_n+1, row, col);
					for (jr = row; jr < last; jr++) {
						values[jr][col] = 0;
					}
				}
				for (jr = row, jd = 0; jr < last; jr++, jd++) {
					if (col+jr == order-1 && diag2_done < 2) {
						sub_sum(&diag2_sum, 1UL, dom->values[jd]);
					}
					if (jr == col && diag1_done < 2) {
						sub_sum(&diag1_sum, 1UL, dom->values[jd]);
					}
				}
				ir--;
				id--;
			}
			while (ir > row) {
				sub_sum(row_sums+ir, 1UL, dom->values[id]);
				ir--;
				id--;
			}
			sub_sum(row_sums+ir, 1UL, dom->values[0]);
			if (dom_len > 1) {
				for (ir = row, id = dom_len; ir < last && add_sum(row_sums+ir, 1UL, dom->values[id-1]); ir++, id--);
				if (ir == last) {
					diag1_done = 2;
					diag2_done = 2;
					for (jr = row, jd = dom_len; jr < last && diag1_done && diag2_done; jr++, jd--) {
						if (jr == col) {
							diag1_done = add_sum(&diag1_sum, 1UL, dom->values[jd-1]);
						}
						if (col+jr == order-1) {
							diag2_done = add_sum(&diag2_sum, 1UL, dom->values[jd-1]);
						}
					}
					if (diag1_done && diag2_done) {
						for (jr = row, jd = dom_len; jr < last; jr++, jd--) {
							values[jr][col] = dom->values[jd-1];
						}
						magic_square(dom_n+1, row, col);
						for (jr = row; jr < last; jr++) {
							values[jr][col] = 0;
						}
					}
					for (jr = row, jd = dom_len; jr < last; jr++, jd--) {
						if (col+jr == order-1 && diag2_done < 2) {
							sub_sum(&diag2_sum, 1UL, dom->values[jd-1]);
						}
						if (jr == col && diag1_done < 2) {
							sub_sum(&diag1_sum, 1UL, dom->values[jd-1]);
						}
					}
					ir--;
					id++;
				}
				while (ir > row) {
					sub_sum(row_sums+ir, 1UL, dom->values[id-1]);
					ir--;
					id++;
				}
				sub_sum(row_sums+ir, 1UL, dom->values[dom_len-1]);
			}
		}
		sub_sum(col_sums+col, dom_len, dom->sum);
		set_used(dom, 0);
	}
}

void set_used(dom_t *dom, int used) {
unsigned long i;
	dom->used = used;
	for (i = 0; i < dom_len; i++) {
		value_used[dom->values[i]-1] = used;
	}
}

int add_sum(sum_t *sum, unsigned long len, unsigned long value) {
unsigned long bound_len, bound, i, j;
	sum->len += len;
	sum->value += value;
	bound_len = order-sum->len;
	bound = 0;
	for (i = 0, j = 0; i < value_max && j < bound_len; i++) {
		if (!value_used[i]) {
			bound += i+1;
			j++;
		}
	}
	if (sum->value+bound <= magic) {
		bound = 0;
		for (i = value_max, j = 0; i && j < bound_len; i--) {
			if (!value_used[i-1]) {
				bound += i;
				j++;
			}
		}
		return sum->value+bound >= magic;
	}
	else {
		return 0;
	}
}

void sub_sum(sum_t *sum, unsigned long len, unsigned long value) {
	sum->len -= len;
	sum->value -= value;
}

void free_values(unsigned long row) {
unsigned long i;
	for (i = 0; i < row; i++) {
		free(values[i]);
	}
	free(values);
}

void free_doms(unsigned long dom_n) {
unsigned long i;
	for (i = 0; i < dom_n; i++) {
		free(doms[i].values);
	}
	free(doms);
}

#include <annot.h>

void main(void)
{
	volatile int sum = 0;

	for (int i = 0; i < 10; i++)
	{
		ANNOT_MAXITER(10);
		sum += i;
	}
}

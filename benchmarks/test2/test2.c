#include <annot.h>

void main(int x)
{
	SWITCH(x)
	{
	case 0:
		x += 1;
		x += 2;
		x += 3;
		x += 4; // 4 ALU ops
		break;

	default:
		x += 1; // 1 ALU op
		break;
	}
}
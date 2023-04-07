#ifdef LEAKS
#include <cstdlib>
#endif

int main()
{
#ifdef LEAKS
	system("leaks a.out");
#endif

	return 0;
}

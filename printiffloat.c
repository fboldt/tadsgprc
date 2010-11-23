#include <stdio.h>

int main()
{
	float f;
	int i;
	char lixo[31];
	scanf(" %30[^\n] %d %f", lixo, &i, &f);
	while(i!=0)
	{
		printf("%f ", f);
		scanf("%d %f",&i, &f);
	}
	printf("\n");
	return 0;
}

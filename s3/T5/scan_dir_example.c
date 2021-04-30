#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	DIR* dip;
	struct dirent* dit;
	int i = 0;

	if(argc < 2)
	{
		printf("Usage: %s <directory>\n", argv[0]);
		return 0;
	}

	if((dip = opendir(argv[1])) == NULL)
	{
		perror("opendir");
		return 0;
	}

	printf("Directory stream is now open\n");

	while((dit = readdir(dip)) != NULL)
	{
		i++;
		printf("\n%s", dit->d_name);
	}

	if(closedir(dip) == -1)
	{
		perror("closedir");
		return 0;
	}

	printf("\nDirectory stream is now closed\n");
	return 1;
}

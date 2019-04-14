#include "shell.h"

void longlist(char *directory, char *filename)
{
		char path[1000];
		strcpy(path,directory);
		strcat(path,"/");
		strcat(path,filename);

		struct stat fileinfo;

		if(stat(path,&fileinfo)<0)
		{
			perror("Error");
			return;
		}

        printf((S_ISDIR(fileinfo.st_mode)) ? "d" : "-");
	    printf((fileinfo.st_mode & S_IRUSR) ? "r" : "-");
	    printf((fileinfo.st_mode & S_IWUSR) ? "w" : "-");
	    printf((fileinfo.st_mode & S_IXUSR) ? "x" : "-");
	    printf((fileinfo.st_mode & S_IRGRP) ? "r" : "-");
	    printf((fileinfo.st_mode & S_IWGRP) ? "w" : "-");
	    printf((fileinfo.st_mode & S_IXGRP) ? "x" : "-");
	    printf((fileinfo.st_mode & S_IROTH) ? "r" : "-");
	    printf((fileinfo.st_mode & S_IWOTH) ? "w" : "-");
	    printf((fileinfo.st_mode & S_IXOTH) ? "x " : "- ");

        struct passwd *pw = getpwuid(fileinfo.st_uid);
		struct group *gr = getgrgid(fileinfo.st_gid);

	    printf("%2lu\t", fileinfo.st_nlink);

	    printf("%s\t", pw->pw_name);
	    printf("%s\t", gr->gr_name);

	    printf("%ld\t",fileinfo.st_size);

		char md[255];
		strftime(md, 24, "\t%b %d %Y %H:%M\t", localtime(&(fileinfo.st_mtime)));

        printf("%s\t%s\n", md, filename);
}

void ls(char *argv1[], int argc1, char* home)
{
    int la[2]; // Flags for -l (la[0]) and -a (la[1])
	la[0] = 0;
	la[1] = 0;
	char **directory_list;
	int dir_count = 0;

    for(int i = 1; i<argc1; i++)
    {
        if(!strcmp(argv1[i],"-l"))
            la[0] = 1;

        else if(!strcmp(argv1[i],"-a"))
            la[1] = 1;

        else if(!strcmp(argv1[i],"-la") || !strcmp(argv1[i],"-al"))
        {
            la[0] = 1;
            la[1] = 1;
        }

		else
			dir_count++;
	}

	int cur_dir_flag = 0;

	if(dir_count == 0)				// If no specific directory is given in the ls arg, ls the pwd
	{
		dir_count++;
		cur_dir_flag = 1;
	}

	directory_list = (char**) malloc(dir_count*sizeof(char*));
	for (int i = 0; i < dir_count; i++)
		directory_list[i] = (char*) malloc(255);

	if(cur_dir_flag == 1)
		directory_list[0] = ".";

	int j = 0;
	for(int i = 1; i < argc1; i++)
	{
		if(strcmp(argv1[i], "-l") != 0 &&
		   strcmp(argv1[i], "-a") != 0 &&
		   strcmp(argv1[i], "-la") != 0 &&
		   strcmp(argv1[i], "-al") != 0)
		   {
			   if(!strcmp(argv1[i],"~"))
			   {
				   directory_list[j++] = home;
				   continue;
			   }
				directory_list[j++] = argv1[i];
		   }
	}

	DIR *open_dir;
	for(int i = 0; i<dir_count; i++)
	{
		open_dir = opendir(directory_list[i]);
		if(!open_dir)
		{
			printf("ls: cannot access '%s': No such directory\n", directory_list[i]);
			continue;
		}

		if(dir_count > 1)
			printf("%s:\n", directory_list[i]);

		struct dirent *file_cwd = readdir(open_dir);
		while(file_cwd != NULL)
		{
			int la_flag = 10*la[0] + la[1];

			switch (la_flag)
			{
				case 0:             //No Flags
				{
					if(file_cwd->d_name[0] != '.')
						printf("%s\n", file_cwd->d_name);
					break;
				}
				case 1:             // -a
				{
					printf("%s\n", file_cwd->d_name);
					break;
				}
				case 10:            // -l
				{
					if(file_cwd->d_name[0] != '.')
						longlist(directory_list[i], file_cwd->d_name);
					break;
				}
				case 11:            // -la
				{
					longlist(directory_list[i], file_cwd->d_name);
					break;
				}
			}

			file_cwd = readdir(open_dir);
		}
		printf("\n");
		closedir(open_dir);
	}
}

/* Tema 1 IOCLA - Frant Madalina 324CB */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

typedef struct Dir
{
	char* name;
	struct Dir* parent;
	struct File* head_children_files;
	struct Dir* head_children_dirs;
	struct Dir* next;
} Dir;

typedef struct File
{
	char* name;
	struct Dir* parent;
	struct File* next;
} File;

/* 1 -> fisierul exista in director, 0 -> altfel */
int file_exists_in_dir (Dir* parent, char* name)
{
	for (File* f = parent->head_children_files; f != NULL; f = f->next)
	{
		if (strcmp(f->name, name) == 0)
			return 1;
	}
	return 0;
}

/* 1 -> directorul exista in director, 0 -> altfel */
int dir_exists_in_dir (Dir* parent, char* name)
{
	for (Dir* d = parent->head_children_dirs; d != NULL; d = d->next)
	{
		if (strcmp(d->name, name) == 0)
			return 1;
	}
	return 0;
}

/* creeaza un fisier in directorul curent */
void touch (Dir* parent, char* name)
{
	if (file_exists_in_dir(parent, name) == 1)
	{
		printf("File already exists\n");
		return;
	}

	/* creare fisier nou */
	File* new_file = (File* )malloc(sizeof(File));
	new_file->name = malloc(strlen(name) * sizeof(char) + 1);
	strcpy(new_file->name, name);
	new_file->parent = parent;
	new_file->next = NULL;

	if (parent->head_children_files == NULL) // lista vida
	{
		/* actualizare inceput lista */
		parent->head_children_files = new_file;
		return;
	}

	File* f; /* parcurgere pana la ultimul element */
	for (f = parent->head_children_files; f->next != NULL; f = f->next);

	f->next = new_file; // adaugare fisier la finalul listei

	return;
}

/* creeaza un director in directorul curent */
void mkdir (Dir* parent, char* name)
{
	if (dir_exists_in_dir(parent, name) == 1)
	{
		printf("Directory already exists\n");
		return;
	}

	/* creare director nou */
	Dir* new_dir = (Dir* )malloc(sizeof(Dir));
	new_dir->name = malloc(strlen(name) * sizeof(char) + 1);
	strcpy(new_dir->name, name);
	new_dir->parent = parent;
	new_dir->head_children_dirs = NULL;
	new_dir->head_children_files = NULL;
	new_dir->next = NULL;

	if (parent->head_children_dirs == NULL) // lista vida
	{
		/* actualizare inceput lista */
		parent->head_children_dirs = new_dir;
		return;
	}

	Dir* d; /* parcurgere pana la ultimul element */
	for (d = parent->head_children_dirs; d->next != NULL; d = d->next);

	d->next = new_dir; // adaugare director la finalul listei

	return;
}

/* afisarea tuturor directoarelor si fisierelor din directorul curent */
void ls (Dir* parent)
{
	for (Dir* d = parent->head_children_dirs; d != NULL; d = d->next)
		printf("%s\n", d->name);
	
	for (File* f = parent->head_children_files; f != NULL; f = f->next)
		printf("%s\n", f->name);
}

/* elimina fisierul din directorul curent */
void rm (Dir* parent, char* name)
{
	if (file_exists_in_dir(parent, name) == 0)
	{
		printf("Could not find the file\n");
		return;
	}

	File* f = parent->head_children_files;
	File* prev = NULL;

	while (f != NULL)
	{
		if (strcmp(f->name, name) == 0)
		{
			if (prev == NULL) // eliminare prim element
				parent->head_children_files = f->next; // actualizare inceput lista
			else
				prev->next = f->next;

			free(f->name);
			free(f);
			return;
		}
		else
		{
			prev = f;
		}
		f = f->next;
	}
}

/* elimina directorul din directorul curent */
void rmdir (Dir* parent, char* name)
{
	if (dir_exists_in_dir(parent, name) == 0)
	{
		printf("Could not find the dir\n");
		return;
	}

	Dir* dir = parent->head_children_dirs;
	Dir* prev = NULL;

	while (dir != NULL)
	{
		if (strcmp(dir->name, name) == 0)
		{
			/* stergerea tuturor fisierelor din interiorul directorului */
			for (File* f = dir->head_children_files; f != NULL; )
			{
				File* next = f->next;
				rm(dir, f->name);
				f = next;
			}
			/* stergerea tuturor directoarelor din interiorul directorului */
			for (Dir* d = dir->head_children_dirs; d != NULL; )
			{
				Dir* next = d->next;
				rmdir(dir, d->name);
				d = next;
			}

			if (prev == NULL) // eliminare prim element
				parent->head_children_dirs = dir->next; // actualizare inceput lista
			else
				prev->next = dir->next;

			free(dir->name);
			free(dir);
			return;
		}
		else
		{
			prev = dir;
		}
		dir = dir->next;
	}
}

/* schimba directorul curent in cel dat ca parametru */
void cd(Dir** target, char* name)
{
	if (strcmp(name, "..") == 0) // directorul curent devine parintele celui curent
	{
		if ((*target)->parent != NULL)
			*target = (*target)->parent;
		
		return;
	}

	if (dir_exists_in_dir(*target, name) == 0)
	{
		printf("No directories found!\n");
		return;
	}

	Dir* d = (*target)->head_children_dirs;
	for (; d != NULL; d = d->next)
	{
		if (strcmp(d->name, name) == 0)
		{
			*target = d;
			return;
		}
	}
}

/* afisare director in forma arborescenta */
void tree (Dir* target, int level)
{
	Dir* d = target->head_children_dirs;
	for (; d != NULL; d = d->next)
	{
		for (int k = level; k != 0; k--)
			printf("    ");
		printf("%s\n", d->name);
		tree(d, level + 1);
	}

	File* f = target->head_children_files;
	for (; f != NULL; f = f->next)
	{
		for (int k = level; k != 0; k--)
			printf("    ");
		printf("%s\n", f->name);
	}
}

/* returneaza calea de la radacina la directorul curent */
char* pwd (Dir* target)
{
	/* initial: path = '\0' */
	char* path = malloc(sizeof(char));
	strcpy(path, "");

	for (Dir* d = target; d != NULL; d = d->parent)
	{
		/* aux = " '/' + d->name + path + '\0' " */
		char* aux = malloc(1 + strlen(d->name) + strlen(path) + 1);
		/* se adauga "/nume_dir" inaintea sirului ce retine calea */
		strcpy(aux, "/");
		strcat(aux, d->name);
		strcat(aux, path);
		path = realloc(path, strlen(aux) * sizeof(char) + 1);
		strcpy(path, aux);
		free(aux);

		if (strcmp(d->name, "home") == 0)
			break;
	}
	
	return path;
}

/* redenumire fisier */
void mv_file(Dir* parent, char* oldname, char* newname)
{
	File* file_to_rename;
	File* f = parent->head_children_files;
	File* prev = NULL;

	if (f->next == NULL) // lista cu un singur element
	{
		strcpy(f->name, newname);
		return;
	}
	/* mutare fisier ce trebuie redenumit la sfarsitul listei */
	while (f != NULL) 
	{
		if (strcmp(f->name, oldname) == 0)
		{
			file_to_rename = f;

			if (prev == NULL)
				parent->head_children_files = f->next;
			else
				prev->next = f->next;
		}
		else
		{
			prev = f;
		}
		f = f->next;
	}
	prev->next = file_to_rename;
	file_to_rename->next = NULL;
	strcpy(file_to_rename->name, newname); // redenumire
}

/* redenumire director */
void mv_dir(Dir* parent, char* oldname, char* newname)
{
	Dir* dir_to_rename;
	Dir* d = parent->head_children_dirs;
	Dir* prev = NULL;

	if (d->next == NULL) // lista cu un singur element
	{
		strcpy(d->name, newname);
		return;
	}
	/* mutare director ce trebuie redenumit la sfarsitul listei */
	while (d != NULL)
	{
		if (strcmp(d->name, oldname) == 0)
		{
			dir_to_rename = d;

			if (prev == NULL)
				parent->head_children_dirs = d->next;
			else
				prev->next = d->next;
		}
		else
		{
			prev = d;
		}
		d = d->next;
	}
	prev->next = dir_to_rename;
	dir_to_rename->next = NULL;
	strcpy(dir_to_rename->name, newname); // redenumire
}

/* schimba numele directorului/fisierului oldname in newname */
void mv(Dir* parent, char* oldname, char* newname)
{
	int old_file_exists = file_exists_in_dir(parent, oldname);
	int old_dir_exists =  dir_exists_in_dir(parent, oldname);

	/* nu exista fisier/director numit oldname */
	if (old_file_exists == 0 && old_dir_exists == 0)
	{
		printf("%s\n", "File/Director not found");
		return;
	}

	int new_file_exists = file_exists_in_dir(parent, newname);
	int new_dir_exists =  dir_exists_in_dir(parent, newname);

	/* exista deja fisier/director numit newname */
	if (new_file_exists == 1 || new_dir_exists == 1)
	{
		printf("%s\n", "File/Director already exists");
		return;
	}

	if (old_file_exists == 1 && new_file_exists == 0)
		mv_file(parent, oldname, newname);

	if (old_dir_exists == 1 && new_dir_exists == 0)
		mv_dir(parent, oldname, newname);

}

/* eliberarea memoriei alocate */
void stop (Dir* target)
{
	/* elimina toate fisierele din interiorul directorului */
	for (File* f = target->head_children_files; f != NULL; )
	{
		File* next = f->next;
		rm(target, f->name);
		f = next;
	}

	/* elimina toate directoarele din interiorul directorului */
	for (Dir* d = target->head_children_dirs; d != NULL; )
	{
		Dir* next = d->next;
		rmdir(target, d->name);
		d = next;
	}

	/* eliberare pentru directorul radacina home */
	free(target->name);
	free(target);
}

int main()
{
	/* creare director radacina home */
	Dir* home_dir = (Dir* )malloc(sizeof(Dir));
	home_dir->name = malloc(strlen("home") * sizeof(char) + 1);
	strcpy(home_dir->name, "home");
	home_dir->parent = NULL;
	home_dir->head_children_dirs = NULL;
	home_dir->head_children_files = NULL;
	home_dir->next = NULL;

	/* director in care se va retine pozitia curenta din sistemul de fisiere */
	Dir* curr_dir = home_dir; 

	char* line = NULL;
	size_t len = 0;

	do
	{
		getline(&line, &len, stdin);
		if (line[strlen(line) - 1] == '\n')
			line[strlen(line) - 1] = '\0';

		char* command = strtok(line, " ");

		if (strcmp(command, "touch") == 0)
		{
			char* name = strtok(NULL, " ");
			touch(curr_dir, name);
		}
		if (strcmp(command, "mkdir") == 0)
		{
			char* name = strtok(NULL, " ");
			mkdir(curr_dir, name);
		}
		if (strcmp(command, "ls") == 0)
		{
			ls(curr_dir);
		}
		if (strcmp(command, "rm") == 0)
		{
			char* name = strtok(NULL, " ");
			rm(curr_dir, name);
		}
		if (strcmp(command, "rmdir") == 0)
		{
			char* name = strtok(NULL, " ");
			rmdir(curr_dir, name);
		}
		if (strcmp(command, "cd") == 0)
		{
			char* name = strtok(NULL, " ");
			cd(&curr_dir, name);
		}
		if (strcmp(command, "tree") == 0)
		{
			tree(curr_dir, 0);
		}
		if (strcmp(command, "pwd") == 0)
		{
			char* path = pwd(curr_dir);
			printf("%s\n", path);
			free(path);
		}
		if (strcmp(command, "mv") == 0)
		{
			char* oldname = strtok(NULL, " ");
			char* newname = strtok(NULL, " ");
			mv(curr_dir, oldname, newname);
		}

	} while (strcmp(line, "stop") != 0);
	/* citire pana se intalneste sirul "stop" */

	stop(home_dir); // eliberare memorie alocata structurilor
	free(line);
	
	return 0;
}
#include "simplefs-ops.h"

extern struct filehandle_t file_handle_array[MAX_OPEN_FILES]; // Array for storing opened files

int i, j;

int increment(int a, int plus)
{
	a = a + plus;
	return a;
}
int decrement(int a, int min)
{
	a = a - min;
	return a;
}

int simplefs_create(char *filename)
{
    /*
	    Create file with name `filename` from disk
	*/
	int check, flag;
	check = flag = 0;
	i=j=0;

	while(i<MAX_FILES)
	{
		struct inode_t inodeptr;
		simplefs_readInode(i, &inodeptr);

		if ((inodeptr.status == INODE_IN_USE) && strcmp(inodeptr.name, filename) == 0)
		{
			flag = 1; return -1;
		}
		else
			check=1;

		i++;
	}
	if (check==1)
	{
		int num = simplefs_allocInode();

		if (num==-1)
			return -1;
		else
		{
			struct inode_t inodeptr;
			inodeptr.status = INODE_IN_USE;

			strcpy(inodeptr.name, filename);
			inodeptr.file_size=0;
			
			while(j<MAX_FILE_SIZE)
			{
				inodeptr.direct_blocks[j]=-1;
				j++;
			}
			simplefs_writeInode(num,&inodeptr);
			return num;
		}
	}
	else
		return -1;
}


void simplefs_delete(char *filename)
{
    /*
	    delete file with name `filename` from disk
	*/
	i=j=0;
	while(i<MAX_FILES)
	{
		j=0;
		struct inode_t inodeptr;
		simplefs_readInode(i, &inodeptr);

		if (inodeptr.status == INODE_IN_USE && strcmp(inodeptr.name, filename) == 0)
		{
			while(j<MAX_FILE_SIZE)
			{
				if (inodeptr.direct_blocks[j]!=-1)
					simplefs_freeDataBlock(inodeptr.direct_blocks[j]);
				j++;
			}
			simplefs_freeInode(i);
			return;
		}
		i++;
	}
}

int simplefs_open(char *filename)
{
    /*
	    open file with name `filename`
	*/
	i=j=0;;
	while(i<MAX_FILES)
	{
		struct inode_t inodeptr;
		simplefs_readInode(i, &inodeptr);

		j=0;
		while(j<MAX_OPEN_FILES)
		{
			if (inodeptr.status == INODE_IN_USE && strcmp(inodeptr.name, filename) == 0 && file_handle_array[j].inode_number == -1)
			{
				file_handle_array[j].offset=0;
				file_handle_array[j].inode_number=i;
				return j;						
			}
			j++;
		}
		i++;
	}
    return -1;
}

void simplefs_close(int file_handle)
{
    /*
	    close file pointed by `file_handle`
	*/
	file_handle_array[file_handle].offset=0;

	file_handle_array[file_handle].inode_number=-1;
}

int simplefs_read(int file_handle, char *buf, int nbytes)
{
    /*
	    read `nbytes` of data into `buf` from file pointed by `file_handle` starting at current offset
	*/
	int off=file_handle_array[file_handle].offset;
	int inode_num=file_handle_array[file_handle].inode_number;

	struct inode_t inodeptr;
	simplefs_readInode(inode_num, &inodeptr);

	int pos = off+nbytes;
	if(pos > 256 || nbytes > inodeptr.file_size)
		return -1;

	char* dum = (char *) malloc(257);
	dum=strcpy(dum,"");

	j=0;
	while(j<MAX_FILE_SIZE)
	{
		if (inodeptr.direct_blocks[j] != -1)
		{
			char* b = (char *) malloc(65);
			simplefs_readDataBlock(inodeptr.direct_blocks[j],b);
			strcat(dum,b);
			// printf("%s\n", b);
		}
		j++;
	}

	int count=0;
	i=off;
	while(i<off+nbytes)
	{
		buf[count]=dum[i];
		count=count+1;

		i++;
	}
    return 0;
}


int simplefs_write(int file_handle, char *buf, int nbytes)
{
    /*
	    write `nbytes` of data from `buf` to file pointed by `file_handle` starting at current offset
	*/
	int off=file_handle_array[file_handle].offset;
	int inode_num=file_handle_array[file_handle].inode_number;

	struct inode_t inodeptr;
	simplefs_readInode(inode_num, &inodeptr);
	
	int pos = off+nbytes;
	if(pos > 256)
		return -1;

	char* b1 = (char *) malloc(65);
	char* b2 = (char *) malloc(65);
	char* b3 = (char *) malloc(65);
	char* b4 = (char *) malloc(65);

	int pages;
	pages = j = 0;

	while(j<MAX_FILE_SIZE)
	{
		if (inodeptr.direct_blocks[j] != -1)
		{
			pages++;
			if(pages>0 && pages < 5)
			{
				if (pages==1)
					simplefs_readDataBlock(inodeptr.direct_blocks[j],b1);
				if (pages==2)
					simplefs_readDataBlock(inodeptr.direct_blocks[j],b2);
				if (pages==3)
					simplefs_readDataBlock(inodeptr.direct_blocks[j],b3);
				if (pages==4)
					simplefs_readDataBlock(inodeptr.direct_blocks[j],b4);
			}
		}
		j++;
	}
	int count=0;
	if (nbytes>0 && off<64 && pages==0)
	{
		int block_num=simplefs_allocDataBlock();
		if (block_num == -1)
			return -1;

		inodeptr.direct_blocks[0]=block_num;
		pages=1;
	}
	if (nbytes>0 && off<64)
	{
		while (off<64 && nbytes>0)
		{
			b1[off]=buf[count];
			count=count+1;
			nbytes=nbytes-1;
			off=off+1;
		}
		// printf("%s\n", buf);
		simplefs_writeDataBlock(inodeptr.direct_blocks[0],b1);
	}

	if (nbytes>0 && off<128 && pages<=1)
	{
		int block_num=simplefs_allocDataBlock();
		if (block_num == -1)
			return -1;
		
		inodeptr.direct_blocks[1]=block_num;
		pages=2;
	}
	if (nbytes>0 && off<128)
	{
		while (off<128 && nbytes>0)
		{
			// printf( "pages %s\n", b2);
			b2[off-64]=buf[count];
			count++;
			nbytes=nbytes-1;
			off=off+1;
		}
		simplefs_writeDataBlock(inodeptr.direct_blocks[1],b2);		
	}

	if (nbytes>0 && off<192 && pages<=2)
	{
		int block_num=simplefs_allocDataBlock();

		if (block_num == -1)
			return -1;

		inodeptr.direct_blocks[2]=block_num;
		pages=3;
	}
	if (nbytes>0 && off<192)
	{
		while (off<192 && nbytes>0)
		{
			b3[off-128]=buf[count];
			count=count+1;
			nbytes=nbytes-1;
			off=off+1;
		}
		simplefs_writeDataBlock(inodeptr.direct_blocks[2],b3);		
	}

	if (nbytes>0 && off<256 && pages<=3)
	{
		int block_num=simplefs_allocDataBlock();
		// printf("block_num %d\n", block_num);
		if (block_num == -1)
			return -1;

		inodeptr.direct_blocks[3]=block_num;
		pages=4;
	}
	if (nbytes>0 && off<256)
	{
		while (off<256 && nbytes>0)
		{
			b4[off-192]=buf[count];
			count++;
			nbytes-=1;
			off+=1;
		}
		simplefs_writeDataBlock(inodeptr.direct_blocks[3],b4);		
	}
	if (off+nbytes > inodeptr.file_size)
		inodeptr.file_size=off+nbytes;

	simplefs_writeInode(inode_num,&inodeptr);

    return 0;
}


int simplefs_seek(int file_handle, int nseek)
{
    /*
	   increase `file_handle` offset by `nseek`
	*/
	int off = file_handle_array[file_handle].offset;

	off=off+nseek;

	if (off<0 || off > 256)
		return -1;
	// printf("offset %d\n", off);
    file_handle_array[file_handle].offset=file_handle_array[file_handle].offset+nseek;
    
    return 0;
}
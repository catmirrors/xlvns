#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

void *
filemap(const char *path, size_t *size)
{
	int fd;
	struct stat sb;
	u_char *addr;
	
	if ((fd = open(path, 0, O_RDONLY)) < 0) {
		perror(path);
		return NULL;
	}

	if (fstat(fd, &sb) < 0 ||
		(addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0))
		== MAP_FAILED) {
		perror("mmap");
		close(fd);
		return NULL;
	}
	close(fd);    /* マップしたので不要 */

	if (size)
		*size = sb.st_size;

	return addr;
}

void
fileunmap(void *addr, size_t size)
{
	munmap(addr, size);
}

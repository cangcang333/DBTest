#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <gnu/lib-names.h>   /* Defines LIBM_SO (which will be a string such as "libm.so.6") */

int main(int argc, char *argv[])
{
	char *path = argv[1];
	printf("path: %s\n", path);
	void *handle;
        double (*cosine)(double);
	char *error;       

	handle = dlopen(LIBM_SO, RTLD_LAZY);
	if (!handle)
	{
		fprintf(stderr, "%s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	dlerror();    /* Clear any existing error */

	cosine = (double (*)(double)) dlsym(handle, "cos");
	/* workaround:
	   *(void **) (&cosine) = dlsym(handle, "cos");
	 */

	error = dlerror();
	if (error != NULL)
	{
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
	}

	printf("%f\n", (*cosine)(2.0));
	printf("%f\n", (cosine)(0));
	printf("%f\n", (*cosine)(3.1415));
	dlclose(handle);

//	return 0;
	exit(EXIT_SUCCESS);
}

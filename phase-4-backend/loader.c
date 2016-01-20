#include "generator/generator-types.h"
#include "generator/generator-globals.h"
#include "loader-macro.h"
#include <dlfcn.h>

static void *lib;
static inline void load_obj(void **obj, char *name)
{
	*obj=dlsym(lib, name);
	if (*obj==NULL) {
		fprintf(stderr, "Internal error: dlfcn returned error: \"%s\" for symbol: \"%s\"\n", dlerror(), name);
		exit(1);
	}
}
#define load_obj2(X, Y) load_obj( (void**) X, Y)

static void (*cleanup_backend2) ();
extern char *backend_name;

void setup_backend()
{
	lib=dlopen(backend_name, RTLD_DEEPBIND|RTLD_NOW);
	if (lib==NULL) {
		fprintf(stderr, "Internal error: can't load lib: %s, got error: %s\n", backend_name, dlerror());
		exit(1);
	}

	void (*go) ();
	load_obj2(&go, "setup_backend");
	go();
	load_obj2(&cleanup_backend2, "cleanup_backend");
	load_obj3;
}

void cleanup_backend()
{
	cleanup_backend2();
	dlclose(lib);
}

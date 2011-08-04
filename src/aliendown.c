#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "markdown.h"
#include "html.h"
#include "autolink.h"

struct aliendown_renderopt {
	struct html_renderopt html;
	int nofollow;
	const char *target;
};

static struct module_state {
	struct sd_callbacks callbacks;
	struct aliendown_renderopt options;
} _state;

/* The module doc strings */
PyDoc_STRVAR(aliendown_module__doc__, "When does the narwhal bacon? At Sundown.");
PyDoc_STRVAR(aliendown_md__doc__, "Render a Markdown document");

static const int aliendown_md_flags =
	MKDEXT_NO_INTRA_EMPHASIS |
	MKDEXT_SUPERSCRIPT |
	MKDEXT_AUTOLINK |
	MKDEXT_STRIKETHROUGH |
	MKDEXT_TABLES;

static const int aliendown_render_flags = 
	HTML_SKIP_HTML |
	HTML_SKIP_IMAGES |
	HTML_SAFELINK;

static void
aliendown_link_attr(struct buf *ob, struct buf *link, void *opaque)
{
	struct aliendown_renderopt *options = opaque;

	if (options->nofollow)
		BUFPUTSL(ob, " rel=\"nofollow\"");

	if (options->target != NULL) {
		BUFPUTSL(ob, " target=\"");
		bufputs(ob, options->target);
		bufputc(ob, '\"');
	}
}

static PyObject *
aliendown_md(PyObject *self, PyObject *args, PyObject *kwargs)
{
	static char *kwlist[] = {"text", "nofollow", "target", NULL};

	struct buf ib, *ob;
	PyObject *py_result;

	memset(&ib, 0x0, sizeof(struct buf));

	/* Parse arguments */
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s#|iz", kwlist,
				&ib.data, &ib.size, &_state.options.nofollow, &_state.options.target)) {
		return NULL;
	}

	/* Output buffer */
	ob = bufnew(128);

	/* do the magic */
	sd_markdown(ob, &ib, aliendown_md_flags, &_state.callbacks, &_state.options);

	/* make a Python string */
	py_result = Py_BuildValue("s#", ob->data, (int)ob->size);

	/* Cleanup */
	bufrelease(ob);
	return py_result;
}

static PyMethodDef aliendown_methods[] = {
	{"py_markdown", (PyCFunction) aliendown_md, METH_VARARGS | METH_KEYWORDS, aliendown_md__doc__},
	{NULL, NULL, 0, NULL} /* Sentinel */
};

PyMODINIT_FUNC initaliendown(void)
{
	PyObject *module;

	module = Py_InitModule3("aliendown", aliendown_methods, aliendown_module__doc__);
	if (module == NULL)
		return;

	sdhtml_renderer(&_state.callbacks,
			(struct html_renderopt *)&_state.options,
			aliendown_render_flags);

	_state.options.html.link_attributes = &aliendown_link_attr;

	/* Version */
	PyModule_AddStringConstant(module, "__version__", "0.1.0");
}

#include "res.h"
#include "res_headers.h"


PyObject *Res_new(Response *response) {
    Res *self = NULL;
    self = (Res *)ResType.tp_alloc(&ResType, 0);
    self->response = response;
    self->code = NULL;
    self->headers = NULL;
    self->body = NULL;
    return (PyObject *)self;
}

void Res_dealloc(Res *self) {
    Py_XDECREF(self->code);
    Py_XDECREF(self->headers);
    Py_XDECREF(self->body);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject *Res_get_code(Res *self, void *closure) {
    if (!self->code) {
        self->code = PyLong_FromUnsignedLong((unsigned long)self->response->code);
    }
    Py_INCREF(self->code);
    return self->code;
}

int Res_set_code(Res *self, PyObject *value, void *closure) {
    Py_INCREF(value);
    self->code = value;
    self->response->code = PyLong_AsUnsignedLong(value);
    return 0;
}

PyObject *Res_get_headers(Res *self, void *closure) {
    if (!self->headers) {
        self->headers = (PyObject *)ResHeaders_new(&self->response->headers);
    }
    Py_INCREF(self->headers);
    return self->headers;
}

PyObject *Res_get_body(Res *self, void *closure) {
    if (self->response->body_len == 0) {
        Py_RETURN_NONE;
    }
    if (!self->body) {
        self->body = PyBytes_FromStringAndSize(self->response->body, self->response->body_len);
    }
    Py_INCREF(self->body);
    return self->body;
}

int Res_set_body(Res *self, PyObject *value, void *closure) {
    if (PyObject_IsInstance(value, &PyUnicode_Type)) {
        if (self->body != NULL) {
            Py_DECREF(self->body);
        }
        self->body = value;
        Py_INCREF(self->body);
        self->response->body = PyUnicode_AsUTF8AndSize(value, &self->response->body_len);
    } else if (PyObject_IsInstance(value, &PyBytes_Type)) {
        if (self->body != NULL) {
            Py_DECREF(self->body);
        }
        self->body = value;
        Py_INCREF(self->body);
        PyBytes_AsStringAndSize(value, &self->response->body, &self->response->body_len);
    }
    return 0;
}

PyGetSetDef Res_getset[] = {
    {"code", (getter)Res_get_code, (setter)Res_set_code, NULL, NULL},
    {"headers", (getter)Res_get_headers, NULL, NULL, NULL},
    {"body", (getter)Res_get_body, (setter)Res_set_body, NULL, NULL},
    {NULL}
};

char *_Res_repr_headers(Res *self) {
    char *headers = malloc(255);
    headers[0] = '\0';
    strcat(headers, "{\n");
    for (size_t i = 0; i < self->response->headers.len; i++) {
        if (i != 0) {
            strcat(headers, ",\n");
        }
        strcat(headers, "        '");
        strcat(headers, self->response->headers.buffer[i].key);
        strcat(headers, "'");
        strcat(headers, ": ");
        strcat(headers, "'");
        strcat(headers, self->response->headers.buffer[i].value);
        strcat(headers, "'");
    }
    strcat(headers, "\n    }");
    return headers;
}

char *_Res_repr_body(Res *self) {
    size_t len = self->response->body_len;
    char *format;
    asprintf(&format, "(%ld bytes)", len);
    return format;
}

PyObject *Res_repr(Res *self) {
    char *format;
    asprintf(&format, "Res {\n    'code': %d,\n    'headers': %s,\n    'body': %s\n}", self->response->code, _Res_repr_headers(self), _Res_repr_body(self));
    return PyUnicode_FromFormat(format);
}

PyTypeObject ResType = {
    PyObject_HEAD_INIT(NULL)
    .tp_name = "thunderlight.Res",
    .tp_basicsize = sizeof(Res),
    .tp_dealloc = (destructor)Res_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_getset = Res_getset,
    .tp_repr = (reprfunc)Res_repr,
    .tp_str = (reprfunc)Res_repr
};

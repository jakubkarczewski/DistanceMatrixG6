
/* Use this file as a template to start implementing a module that
   also declares object types. All occurrences of 'DistanceMatrix' should be changed
   to something reasonable for your objects. After that, all other
   occurrences of 'simple_graphs' should be changed to something reasonable for your
   module. If your module is named foo your sourcefile should be named
   foomodule.c.

   You will probably want to delete all references to 'x_attr' and add
   your own types of attributes instead.  Maybe you want to name your
   local variables other than 'self'.  If your object type is needed in
   other files, you'll have to create a file "foobarobject.h"; see
   floatobject.h for an example. */

/* DistanceMatrix objects */

#include "Python.h"
#include "structmember.h"
#include "stdlib.h"
#include "stdio.h"
#include <string.h>

static PyObject *G6ErrorObject;
static PyObject *NoVerticesErrorObject;
static PyObject *TooManyVerticesErrorObject;

#define MAX 16
#define INFINITY 9999

typedef struct {
    PyObject_HEAD
    long order_attr;
    int dist_matrix[MAX][MAX];
} DistanceMatrixObject;


void dijkstra(int G[MAX][MAX], int D[MAX][MAX], int n, int startnode)
{
    int cost[MAX][MAX], distance[MAX], pred[MAX];
    int visited[MAX], count, mindistance, nextnode, i, j;
    nextnode=0;
    
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            cost[i][j] = G[i][j]==0 ? INFINITY : G[i][j];
        }     
    }
    
    for(i=0;i<n;i++)
    {
        distance[i]=cost[startnode][i];
        pred[i]=startnode;
        visited[i]=0;
    }
    
    distance[startnode]=0;
    visited[startnode]=1;
    count=1;
    
    while(count<n-1)
    {
        mindistance=INFINITY;
        
        for(i=0;i<n;i++)
            if(distance[i]<mindistance&&!visited[i])
            {
                mindistance=distance[i];
                nextnode=i;
            }
            
            visited[nextnode]=1;
            for(i=0;i<n;i++)
                if(!visited[i])
                    if(mindistance+cost[nextnode][i]<distance[i])
                    {
                        distance[i]=mindistance+cost[nextnode][i];
                        pred[i]=nextnode;
                    }
        count++;
    }
 
    for(i=0;i<n;i++)
    {
        D[startnode][i] = distance[i];
    }
}

void dijkstra_ext(int D[MAX][MAX], int n)
{
    int G[MAX][MAX];
    for(int i=0; i < n; i++)
    {
        for(int j=0; j < n; j++)
        {
            G[i][j] = D[i][j] == 1 ? 1 : 0;
        }
    }
 
    for(int i=0; i<n; i++)
    {
        dijkstra(G, D, n, i);
    }
}

static PyTypeObject DistanceMatrix_Type;

#define DistanceMatrixObject_Check(v)      (Py_TYPE(v) == &DistanceMatrix_Type)


static DistanceMatrixObject *
newDistanceMatrixObject(PyObject *arg)
{
    DistanceMatrixObject *self;
    self = PyObject_New(DistanceMatrixObject, &DistanceMatrix_Type);
    if (self == NULL)
        return NULL;
    return self;
}

/* DistanceMatrix methods */

static void
DistanceMatrix_dealloc(DistanceMatrixObject *self)
{
    PyObject_Del(self);
}

static PyObject *
DistanceMatrix_order(DistanceMatrixObject *self)
{
    return PyLong_FromLong(self->order_attr);
}

static PyObject *
DistanceMatrix_addVertex(DistanceMatrixObject *self)
{
    if(self->order_attr == MAX)
    {
        PyErr_SetString(TooManyVerticesErrorObject, "too many vertices");
        return NULL;
    }
    self->order_attr++;

    dijkstra_ext(self->dist_matrix, self->order_attr);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
DistanceMatrix_deleteVertex(DistanceMatrixObject *self, PyObject *args)
{
    long u;
    if (!PyArg_ParseTuple(args, "l|", &u))
        return NULL;

    if(self->order_attr == 1)
    {
        PyErr_SetString(NoVerticesErrorObject, "graph must have vertices");
        return NULL;
    }

    self->order_attr -= 1;

    for(int i=0; i <= self->order_attr; i++)
    {
        self->dist_matrix[self->order_attr][i] = INFINITY;
        self->dist_matrix[i][self->order_attr] = INFINITY;
    }

    dijkstra_ext(self->dist_matrix, self->order_attr);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
DistanceMatrix_isEdge(DistanceMatrixObject *self, PyObject *args)
{
    long u, v;
    if (!PyArg_ParseTuple(args, "ll|", &u, &v))
        return NULL;

    long rv;
    rv = self->dist_matrix[u][v] == 1 ? 1 : 0;

    return PyBool_FromLong(rv);
}

static PyObject *
DistanceMatrix_addEdge(DistanceMatrixObject *self, PyObject *args)
{
    long u, v;
    if (!PyArg_ParseTuple(args, "ll|", &u, &v))
        return NULL;
   
    self->dist_matrix[u][v] = 1;
    self->dist_matrix[v][u] = 1;

    dijkstra_ext(self->dist_matrix, self->order_attr);
    Py_INCREF(Py_None);
    return Py_None;
}


static PyObject *
DistanceMatrix_deleteEdge(DistanceMatrixObject *self, PyObject *args)
{
    long u, v;
    if (!PyArg_ParseTuple(args, "ll|", &u, &v))
        return NULL;
    
    self->dist_matrix[u][v] = INFINITY;
    self->dist_matrix[v][u] = INFINITY;

    dijkstra_ext(self->dist_matrix, self->order_attr);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject*
fromStringLogic(DistanceMatrixObject *self, char* text)
{

    int text_len = (int)strlen(text);
    for(int i=0; i < text_len; i++)
    {
        if((int)text[i] == 32)
        {
            PyErr_SetString(G6ErrorObject, "too long text");
            return NULL;
        }
    }


    char n_char = text[0];
    long n_long = (long)n_char - 63;
    int k = 0;

    if(n_long < 1 || n_long > MAX)
    {
        char msg[28];
        sprintf(msg, "wrong order: %ld", n_long);
        PyErr_SetString(G6ErrorObject, msg);
        return NULL;
    }

    self->order_attr = n_long;

    int text_index = 1;

    int temp_int;
    for(int i=1; i < n_long; i++)
    {
        for(int j=0; j < i; j++)
        {
            if(k == 0)
            {
                if(text_index < text_len)
                {
                    char temp_char = text[text_index];
                    text_index++;
                    temp_int = (int) temp_char - 63;


                    if(temp_int < 0 || temp_int > 63)
                    {
                        char msg[28];
                        sprintf(msg, "wrong character: %d", temp_int + 63);
                        PyErr_SetString(G6ErrorObject, msg);
                        return NULL;
                    }

                    k = 6;


                } else
                {

                    PyErr_SetString(G6ErrorObject, "too short text");
                    return NULL;

                }
                 
            }
            k-=1;
            if((temp_int & (1 << k)) != 0)
            {
                self->dist_matrix[i][j] = 1;
                self->dist_matrix[j][i] = 1;

            }
            
        }
    }

    if(text_len - text_index > 1)
    {

        PyErr_SetString(G6ErrorObject, "too long text");
        return NULL;

    }        



    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
DistanceMatrix_fromString(DistanceMatrixObject *self, PyObject *args)
{
    char* text;
    if (!PyArg_ParseTuple(args, "s|", &text))
        return NULL;

    PyObject *status = fromStringLogic(self, text);
    if(status == NULL){
        return NULL;
    }

    dijkstra_ext(self->dist_matrix, self->order_attr);

    Py_INCREF(Py_None);
    return Py_None;
}

void append(char* s, char c)
{
    int len = strlen(s);
    s[len] = c;
    s[len+1] = '\0';
}

static PyObject *
DistanceMatrix_str(DistanceMatrixObject *self)
{

    char text[256];
    char n = (char)(self->order_attr + 63);
    text[0] = n;
    int k = 5;
    int c = 0;
    for(int i=0; i<self->order_attr; i++)
    {
        for(int j=0; j< i; j++)
        {
            if(self->dist_matrix[i][j] == 1) 
            {

                c |= (1<<k);
            }

            if(k==0)
            {

                append(text, (char)(c + 63));
                k = 6;
                c = 0;
            }
            
            k-=1;
        }
    }
    if(k!=5)
    {
        append(text, (char)(c + 63));
    }

    return PyUnicode_FromString(text);
}

static PyObject *
DistanceMatrix_richcompare(DistanceMatrixObject *self, DistanceMatrixObject *other, int op)
{
    // here we will make sure we are comparing 2 adjacency matrices
    PyObject *result = NULL;

    int self_adj_matrix[MAX][MAX], other_adj_matrix[MAX][MAX];

    switch (op) {
    case Py_LT:
        result = Py_False;
        break;
    case Py_LE:
        result = Py_False;
        break;
    case Py_EQ:
        
        if(self->order_attr !=  other->order_attr)
        {
            result = Py_False;
            break;
        }

        for(int i=0; i < self->order_attr; i++)
        {
            for(int j=0; j < self->order_attr; j++)
            {
                self_adj_matrix[i][j] = self->dist_matrix[i][j] == 1 ? 1 : 0;
                other_adj_matrix[i][j] = other->dist_matrix[i][j] == 1 ? 1 : 0;
            }
        }

        for(int i=0; i < self->order_attr; i++)
        {
            for(int j=0; j < i; j++)
            {
                if(self_adj_matrix[i][j] != other_adj_matrix[i][j])
                {
                    result = Py_False;
                    break;
                }
            }
        }

        result = Py_True;
        break;

    case Py_NE:
        
        if(self->order_attr !=  other->order_attr)
        {            
            result = Py_True;
            break;
        }

        for(int i=0; i < self->order_attr; i++)
        {
            for(int j=0; j < self->order_attr; j++)
            {
                self_adj_matrix[i][j] = self->dist_matrix[i][j] == 1 ? 1 : 0;
                other_adj_matrix[i][j] = other->dist_matrix[i][j] == 1 ? 1 : 0;
            }
        }

        for(int i=0; i < self->order_attr; i++)
        {
            for(int j=0; j < i; j++)
            {
                if(self_adj_matrix[i][j] != other_adj_matrix[i][j])
                {
                    result = Py_True;
                    break;
                }
            }
        }

        result = Py_False;
        break;
    case Py_GT:
        result = Py_False;  
        break;
    case Py_GE:
        result = Py_False;
        break;
    }


    Py_XINCREF(result);
    return result;
}

int DistanceMatrix_init(DistanceMatrixObject *self, PyObject *args, PyObject *kwds)
{
   char* text;
   text = "@";
   static char *kwdlist[] = {"text", NULL};
   if (!PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwdlist, &text))
   {
        return -1;
    }


    for(int i=0; i < MAX; i++)
    {
        for(int j=0; j < MAX; j++)
        {
            self->dist_matrix[i][j] = INFINITY;
        }
    }

    PyObject* status = fromStringLogic(self, text);
    if(status == NULL){
        return -1;
    }
    dijkstra_ext(self->dist_matrix, self->order_attr);

    
    return 0;
}



static PyMethodDef DistanceMatrix_methods[] = {
    {"order",
     (PyCFunction)DistanceMatrix_order,
      METH_NOARGS,
      PyDoc_STR("Returns order.")},
    {"addVertex",
     (PyCFunction)DistanceMatrix_addVertex,
      METH_NOARGS,
      PyDoc_STR("Adds vertex.")},
    {"deleteVertex",
     (PyCFunction)DistanceMatrix_deleteVertex,
      METH_VARARGS,
      PyDoc_STR("Deletes vertex.")},
    {"isEdge",
     (PyCFunction)DistanceMatrix_isEdge,
      METH_VARARGS,
      PyDoc_STR("Checks if there is an edge.")},
    {"addEdge",
     (PyCFunction)DistanceMatrix_addEdge,
      METH_VARARGS,
      PyDoc_STR("Adds an edge.")},
    {"deleteEdge",
     (PyCFunction)DistanceMatrix_deleteEdge,
      METH_VARARGS,
      PyDoc_STR("Deletes an edge.")},
    {"fromString",
     (PyCFunction)DistanceMatrix_fromString,
      METH_VARARGS,
      PyDoc_STR("Builds graph from string.")},
    {NULL,NULL}           /* sentinel */
};



static PyTypeObject DistanceMatrix_Type = {
    /* The ob_type field must be initialized in the module init function
     * to be portable to Windows without using C++. */
    PyVarObject_HEAD_INIT(NULL, 0)
    "simple_graphs.DistanceMatrix",             /*tp_name*/
    sizeof(DistanceMatrixObject),          /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    /* methods */
    (destructor)DistanceMatrix_dealloc,    /*tp_dealloc*/
    0,                          /*tp_print*/
    0,             /*tp_getattr*/
    0,   /*tp_setattr*/
    0,                          /*tp_reserved*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    0,                          /*tp_as_sequence*/
    0,                          /*tp_as_mapping*/
    0,                          /*tp_hash*/
    0,                          /*tp_call*/
    (reprfunc)DistanceMatrix_str,                          /*tp_str*/
    0, /*tp_getattro*/
    0,                          /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,         /*tp_flags*/
    0,                          /*tp_doc*/
    0,                          /*tp_traverse*/
    0,                          /*tp_clear*/
    (richcmpfunc)DistanceMatrix_richcompare,                          /*tp_richcompare*/
    0,                          /*tp_weaklistoffset*/
    0,                          /*tp_iter*/
    0,                          /*tp_iternext*/
    DistanceMatrix_methods,                /*tp_methods*/
    0,                          /*tp_members*/
    0,                          /*tp_getset*/
    0,                          /*tp_base*/
    0,                          /*tp_dict*/
    0,                          /*tp_descr_get*/
    0,                          /*tp_descr_set*/
    0,                          /*tp_dictoffset*/
    (initproc)DistanceMatrix_init,                          /*tp_init*/
    0,                          /*tp_alloc*/
    (newfunc)newDistanceMatrixObject,                          /*tp_new*/
    0,                          /*tp_free*/
    0,                          /*tp_is_gc*/
};
/* --------------------------------------------------------------------- */

PyDoc_STRVAR(module_doc,
"This is a template module just for instruction.");

static struct PyModuleDef simple_graphsmodule = {
    PyModuleDef_HEAD_INIT,
    "simple_graphs",
    module_doc,
    -1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/* Export function for the module (*must* be called PyInit_simple_graphs) */

PyMODINIT_FUNC
PyInit_simple_graphs(void)
{
    if (PyType_Ready(&DistanceMatrix_Type) < 0)
        return NULL;

    PyObject* m = PyModule_Create(&simple_graphsmodule);
    
    if(m==NULL)
        return NULL;

    Py_INCREF(&DistanceMatrix_Type);
  
    TooManyVerticesErrorObject = PyErr_NewException("DistanceMatrix.TooManyVerticesError", NULL, NULL);    
    NoVerticesErrorObject = PyErr_NewException("DistanceMatrix.NoVerticesError", NULL, NULL);
    G6ErrorObject = PyErr_NewException("DistanceMatrix.G6Error", NULL, NULL);

    PyDict_SetItemString(DistanceMatrix_Type.tp_dict, "TooManyVerticesError", TooManyVerticesErrorObject);
    PyDict_SetItemString(DistanceMatrix_Type.tp_dict, "NoVerticesError", NoVerticesErrorObject);
    PyDict_SetItemString(DistanceMatrix_Type.tp_dict, "G6Error", G6ErrorObject);

    PyModule_AddObject(m, "DistanceMatrix", (PyObject *)&DistanceMatrix_Type);

    return m;
}
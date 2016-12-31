import struct

def makepattern(arr):
    params = {}
    for a in arr:
        if isinstance(a, basestring):
            args = a[1:].split("_")
            params[args[0]] = int(args[1])
    def fn(return_keys=None, **kwargs):
        if return_keys is None:
            myparams = dict(params)
            myarr = list(arr)
            for (k, v) in kwargs.iteritems():
                myparams[k] = v
            for i, a in enumerate(myarr):
                if isinstance(a, basestring):
                    args = a[1:].split("_")
                    myarr[i] = myparams[args[0]]
            prog = struct.pack( "<" + "h" * len(myarr), *myarr)
            l = len(myarr)
            return "$PROG" + chr(l) + prog
        else:
            return params
    return fn

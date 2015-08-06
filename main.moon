Nffi=require'nMoonLib.ffi'
Xu=Nffi.cdef './x11_utils.so'

Xu.sendFakeKey 'H', Xu.masks.shift, 2
Xu.sendFakeString 'ello World\n'

# cmake/lib/CMakeLists.txt
add_library(c SHARED c.c)
add_library(b STATIC b.c) # 静态库是默认的，此处 STATIC 可以不写
add_library(d SHARED d.c)
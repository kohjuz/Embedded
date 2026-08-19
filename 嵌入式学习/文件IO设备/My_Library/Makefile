# ==================================================================
# Makefile — 顶层入口, 自动进入 io_lib 子目录编译
#
# 用法 (在 My_Library/ 目录下):
#   make          编译动态库 + 测试程序
#   make test     只编译测试程序
#   make run      运行测试 (make run IMAGE=图片.bmp)
#   make clean    清理
#   make install  安装到系统 (需 sudo)
# ==================================================================

# 子目录名
SUBDIR := io_lib

# 透传给子目录 Makefile 的变量 (比如交叉编译时: make CC=arm-linux-gnueabihf-gcc)
export

# 默认: 进入 io_lib 编译
all clean test install uninstall run:
	@$(MAKE) -C $(SUBDIR) $@

.PHONY: all clean test install uninstall run

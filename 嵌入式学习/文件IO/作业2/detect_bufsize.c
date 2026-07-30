#include <stdio.h>

/*
  原理：写文件时标准 I/O 是“全缓冲”。
  每 fputc 一个字节先塞进用户态 buffer，OS 文件大小不变。
  buffer 塞满后一次性 write() 整个 buffer —— OS 文件大小“跳”一整格。
  用另一个“只读句柄”的 ftell(SEEK_END) 看 OS 文件大小：
    首次 > 0 时，那格跳了多大 = buffer 大小。
    已写字节数(触发时) = bufsize + 1（第 bufsize 个字节填满 buffer，
    下一个字节才逼出 flush）。
*/
long probe(const char *path, FILE *fp) {
    FILE *rfp = fopen(path, "rb");   // 只读句柄，用来观测 OS 文件大小
    if (!rfp) printf("    (警告：无法打开只读句柄观测)\n");

    int count = 0;
    long first_jump = 0;
    int trigger = -1;

    for (int i = 0; i < 500000; i++) {
        fputc('x', fp);
        count++;
        long sz = 0;
        if (rfp) {
            fseek(rfp, 0, SEEK_END);
            sz = ftell(rfp);
        }
        if (sz > 0) {
            first_jump = sz;     // 第一次落盘，跳了多大
            trigger = count;     // 此时已写多少字节
            break;
        }
    }
    if (rfp) fclose(rfp);
    printf("    已写字节数(触发落盘) = %d\n", trigger);
    printf("    首次磁盘跳跃幅度     = %ld 字节\n", first_jump);
    return first_jump;
}

int main(void) {
    const char *path = "probe.txt";

    /* 第一部分：默认缓冲（不调 setvbuf，让库自己定大小）*/
    printf("== 默认缓冲探测 ==\n");
    printf("    BUFSIZ 宏 = %d\n", BUFSIZ);
    FILE *fp = fopen(path, "w");
    if (!fp) { perror("fopen"); return 1; }
    long def = probe(path, fp);
    fclose(fp);
    printf("    => 推断默认缓冲区大小 ≈ %ld 字节\n\n", def);

    /* 第二部分：自己用 setvbuf 设 8 字节，验证机制 */
    printf("== 自定义 8 字节缓冲（setvbuf）==\n");
    char mybuf[8];
    fp = fopen(path, "w");
    if (!fp) { perror("fopen"); return 1; }
    if (setvbuf(fp, mybuf, _IOFBF, sizeof(mybuf)) != 0) {
        printf("    setvbuf 失败\n");
    } else {
        long custom = probe(path, fp);
        printf("    => 探测到 %ld 字节（期望 8）\n", custom);
    }
    fclose(fp);

    return 0;
}

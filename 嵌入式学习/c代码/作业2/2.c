#include <stdio.h>

int main() {
    // 定义常量
    const double DENSITY = 1000.0;       // 水密度：g/L
    const double MOLAR_MASS = 18.015;    // 水摩尔质量：g/mol
    const double AVOGADRO = 6.02214076e23; // 阿伏伽德罗常数

    double volume; // 输入：升数
    double molecules; // 计算结果：分子数

    printf("请输入纯净水的体积（升）：");
    scanf("%lf", &volume);

    // 计算分子数
    molecules = (volume * DENSITY / MOLAR_MASS) * AVOGADRO;

    // 科学计数法输出（保留4位有效数字）
    printf("%.4e 个水分子\n", molecules);

    return 0;
}
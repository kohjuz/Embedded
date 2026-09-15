#include <iostream>

using namespace std;

int main()
{
    const float mol = 3E-26;

    float N;
    cout << "请输入水的质量(单位:千克):" << endl;
    cin >> N; // N升水=N千克水

    // 使用科学计数法，输出数据
    cout.setf(ios_base::floatfield, ios_base::scientific);

    cout << "总共水分子个数:" << N/mol << endl;

    return 0;
}

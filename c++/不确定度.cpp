#include<iostream>
#include<cmath>
#include<iomanip>
#include<vector>
#include<string>
#include<limits>   
#include<algorithm>

using namespace std;

// ===================== 全局配置 =====================
const int PRECISION = 4;  
const double PI = acos(-1.0);    
const bool SHOW_PERCENT = true;   
// ===================== 工具函数 =====================
//@brief 清空输入缓冲区（处理输入错误） 
void clear_input_buffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
}

/**
 * @brief 安全读取数值（带输入验证）
 * @tparam T 数值类型
 * @param prompt 提示信息
 * @return 合法的数值
 */
template <typename T>
T safe_input(const string& prompt) {
    T value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            clear_input_buffer(); // 清除多余输入
            return value;
        }
        cout << "输入错误！请输入有效的数值。" << endl;
        clear_input_buffer(); // 清空错误输入
    }
}

/**
 * @brief 计算数组的平均值
 * @param data 数据向量
 * @return 平均值
 */
double mean(const vector<double>& data) {
    if (data.empty()) return 0.0;
    double sum = 0.0;
    for (double x : data) sum += x;
    return sum / data.size();
}

/**
 * @brief 计算数组的中位数（扩展功能）
 * @param data 数据向量
 * @return 中位数
 */
double median(vector<double> data) {
    if (data.empty()) return 0.0;
    sort(data.begin(), data.end());
    int n = data.size();
    if (n % 2 == 1) {
        return data[n / 2];
    } else {
        return (data[n / 2 - 1] + data[n / 2]) / 2.0;
    }
}

/**
 * @brief 贝塞尔公式计算样本标准差 (n-1)
 * @param data 数据向量
 * @param avg 平均值（避免重复计算）
 * @return 样本标准差
 */
double std_dev(const vector<double>& data, double avg) {
    if (data.size() <= 1) return 0.0; // 避免除以0
    double sum_sq = 0.0;
    int n = data.size();
    for (double x : data) {
        sum_sq += pow(x - avg, 2);
    }
    return sqrt(sum_sq / (n - 1));
}

/**
 * @brief 计算极差（扩展功能）
 * @param data 数据向量
 * @return 极差（最大值-最小值）
 */
double range(const vector<double>& data) {
    if (data.empty()) return 0.0;
    double min_val = *min_element(data.begin(), data.end());
    double max_val = *max_element(data.begin(), data.end());
    return max_val - min_val;
}

/**
 * @brief B类不确定度计算（扩展功能：支持不同分布类型）
 * @param delta 仪器最大允许误差
 * @param dist_type 分布类型：1-均匀分布(默认)，2-正态分布，3-三角分布
 * @return B类标准不确定度
 */
double b_class_uncertainty(double delta, int dist_type = 1) {
    switch (dist_type) {
        case 1: // 均匀分布（最常用）
            return delta / sqrt(3.0);
        case 2: // 正态分布（置信概率99.73%）
            return delta / 3.0;
        case 3: // 三角分布
            return delta / sqrt(6.0);
        default:
            return delta / sqrt(3.0);
    }
}

// ===================== 核心功能模块 =====================
/**
 * @brief 直接测量 - 合成标准不确定度 (A类 + B类)
 * 优化点：
 * 1. 增加A类评定的更多方式（极差法）
 * 2. B类不确定度支持自动计算（基于仪器误差）
 * 3. 增加数据有效性验证
 * 4. 输出更多统计信息（中位数、极差）
 */
void direct_measurement_calc() {
    cout << "\n===== 直接测量不确定度计算 =====" << endl;
    
    // 选择A类评定方式
    int a_choice;
    cout << "\n请选择A类标准不确定度评定方式:\n";
    cout << "1. 手动输入A类标准不确定度\n";
    cout << "2. 输入多次测量数据，贝塞尔公式计算（n-1）\n";
    cout << "3. 输入多次测量数据，极差法计算\n";
    a_choice = safe_input<int>("输入选项 (1/2/3): ");

    double uA = 0.0;
    double avg_value = 0.0;
    vector<double> data;

    // A类不确定度计算
    if (a_choice == 2 || a_choice == 3) {
        int n = safe_input<int>("输入测量次数 n (n≥2): ");
        while (n < 2) {
            cout << "测量次数不能小于2！" << endl;
            n = safe_input<int>("重新输入测量次数 n: ");
        }

        data.resize(n);
        cout << "请依次输入 " << n << " 个测量值 (用空格或回车分隔):" << endl;
        for (int i = 0; i < n; ++i) {
            data[i] = safe_input<double>("第" + to_string(i+1) + "个值: ");
        }

        // 基础统计量
        avg_value = mean(data);
        double med = median(data);
        double r = range(data);

        // 不同A类计算方式
        if (a_choice == 2) { // 贝塞尔公式
            double s = std_dev(data, avg_value);
            uA = s / sqrt(n);
            cout << fixed << setprecision(PRECISION);
            cout << "\n--- 统计信息 ---" << endl;
            cout << "测量平均值: " << avg_value << endl;
            cout << "中位数: " << med << endl;
            cout << "样本标准差 s: " << s << endl;
            cout << "A类标准不确定度 u_A = s/√n: " << uA << endl;
        } else { // 极差法
            // 极差系数（参考物理实验规范）
            double c_n;
            if (n <= 10) {
                double c_table[] = {0, 0, 1.13, 1.69, 2.06, 2.33, 2.53, 2.70, 2.85, 2.97, 3.08};
                c_n = c_table[n];
            } else {
                c_n = sqrt(n) * (1 - 1/(4*n) + 7/(96*n*n)); // 近似公式
            }
            uA = r / c_n;
            cout << fixed << setprecision(PRECISION);
            cout << "\n--- 统计信息 ---" << endl;
            cout << "测量平均值: " << avg_value << endl;
            cout << "中位数: " << med << endl;
            cout << "极差 R: " << r << endl;
            cout << "极差系数 c_n: " << c_n << endl;
            cout << "A类标准不确定度 u_A = R/c_n: " << uA << endl;
        }
    } else { // 手动输入A类
        uA = safe_input<double>("输入A类标准不确定度 u_A: ");
        avg_value = safe_input<double>("输入测量平均值: ");
    }

    // B类不确定度计算（支持自动/手动）
    cout << "\n--- B类标准不确定度 ---" << endl;
    int b_choice = safe_input<int>("选择B类计算方式:\n1. 手动输入 u_B\n2. 基于仪器误差自动计算\n输入选项: ");
    double uB = 0.0;

    if (b_choice == 2) {
        double delta = safe_input<double>("输入仪器最大允许误差 Δ: ");
        int dist = safe_input<int>("选择分布类型:\n1. 均匀分布（默认，如仪器误差）\n2. 正态分布（3σ）\n3. 三角分布\n输入选项: ");
        uB = b_class_uncertainty(delta, dist);
        cout << "B类标准不确定度 u_B = Δ/" << (dist==1?"√3":(dist==2?"3":"√6")) << " = " << uB << endl;
    } else {
        uB = safe_input<double>("输入B类标准不确定度 u_B: ");
    }

    // 合成标准不确定度
    double uC = sqrt(uA * uA + uB * uB);
    double ur = uC / avg_value;

    // 输出最终结果
    cout << "\n【直接测量最终结果】" << endl;
    cout << fixed << setprecision(PRECISION);
    cout << "合成标准不确定度 u_c: " << uC << endl;
    cout << "相对标准不确定度 u_r: " << ur;
    if (SHOW_PERCENT) cout << " (" << ur * 100 << "%)";
    cout << endl;
    cout << "测量结果表达式: x = " << avg_value << " ± " << uC << endl;
}

/**
 * @brief 间接测量 - 不确定度传递
 * 优化点：
 * 1. 支持任意数量的变量（不再限制2个）
 * 2. 支持加减、乘除、幂、三角函数等更多模型
 * 3. 分步显示计算过程，更易理解
 */
void indirect_measurement_calc() {
    cout << "\n===== 间接测量不确定度传递 =====" << endl;
    cout << "支持模型：y = f(x1, x2, ..., xn) （加减、乘除、幂、三角函数）" << endl;

    // 选择计算模型
    int model_choice;
    cout << "\n选择不确定度传递模型:\n";
    cout << "1. 乘除/幂模型 (y = k*x1^p1*x2^p2*...)\n";
    cout << "2. 加减模型 (y = x1 ± x2 ± ...)\n";
    cout << "3. 三角函数模型 (y = sin(x)/cos(x)/tan(x)，x为弧度)\n";
    model_choice = safe_input<int>("输入选项: ");

    if (model_choice == 1) { // 乘除/幂模型
        double k = safe_input<double>("输入常数系数 k (无则输入1): ");
        int n_vars = safe_input<int>("输入变量个数 n: ");
        while (n_vars < 1) {
            cout << "变量个数不能小于1！" << endl;
            n_vars = safe_input<int>("重新输入变量个数 n: ");
        }

        // 存储变量信息：测量值、不确定度、指数
        vector<double> x(n_vars), u(n_vars), p(n_vars);
        double y = k; // 最终测量结果
        double ur_sq = 0.0; // 相对不确定度的平方和

        // 输入每个变量的信息
        for (int i = 0; i < n_vars; ++i) {
            cout << "\n--- 变量 x" << (i+1) << " ---" << endl;
            x[i] = safe_input<double>("输入测量值 x" + to_string(i+1) + ": ");
            u[i] = safe_input<double>("输入合成不确定度 u" + to_string(i+1) + ": ");
            p[i] = safe_input<double>("输入指数 p" + to_string(i+1) + " (除法则输入负数): ");
            
            // 计算y和相对不确定度项
            y *= pow(x[i], p[i]);
            double term = p[i] * (u[i] / x[i]);
            ur_sq += term * term;

            // 显示中间过程
            cout << fixed << setprecision(PRECISION);
            cout << "项 " << (i+1) << ": (" << p[i] << "*" << u[i] << "/" << x[i] << ")? = " << term*term << endl;
        }

        // 最终计算
        double ur = sqrt(ur_sq);
        double uy = y * ur;

        // 输出结果
        cout << "\n【间接测量最终结果】" << endl;
        cout << fixed << setprecision(PRECISION);
        cout << "测量结果 y = " << k << "×";
        for (int i = 0; i < n_vars; ++i) {
            cout << "x" << (i+1) << "^" << p[i];
            if (i < n_vars-1) cout << "×";
        }
        cout << " = " << y << endl;
        cout << "相对标准不确定度 u_r: " << ur;
        if (SHOW_PERCENT) cout << " (" << ur * 100 << "%)";
        cout << endl;
        cout << "合成标准不确定度 u_y: " << uy << endl;
        cout << "最终表达式: y = " << y << " ± " << uy << endl;

    } else if (model_choice == 2) { // 加减模型
        int n_vars = safe_input<int>("输入变量个数 n: ");
        while (n_vars < 1) {
            cout << "变量个数不能小于1！" << endl;
            n_vars = safe_input<int>("重新输入变量个数 n: ");
        }

        vector<double> x(n_vars), u(n_vars), sign(n_vars);
        double y = 0.0;
        double uy_sq = 0.0;

        // 输入每个变量的信息
        for (int i = 0; i < n_vars; ++i) {
            cout << "\n--- 变量 x" << (i+1) << " ---" << endl;
            x[i] = safe_input<double>("输入测量值 x" + to_string(i+1) + ": ");
            u[i] = safe_input<double>("输入合成不确定度 u" + to_string(i+1) + ": ");
            sign[i] = safe_input<int>("输入符号 (1=+, -1=-): ");
            while (sign[i] != 1 && sign[i] != -1) {
                cout << "符号只能是1或-1！" << endl;
                sign[i] = safe_input<int>("重新输入符号: ");
            }

            // 计算y和不确定度平方和（加减模型：直接平方和）
            y += sign[i] * x[i];
            uy_sq += u[i] * u[i];

            // 显示中间过程
            cout << fixed << setprecision(PRECISION);
            cout << "u" << (i+1) << "? = " << u[i]*u[i] << endl;
        }

        // 最终计算
        double uy = sqrt(uy_sq);
        double ur = uy / abs(y); // 相对不确定度

        // 输出结果
        cout << "\n【间接测量最终结果】" << endl;
        cout << fixed << setprecision(PRECISION);
        cout << "测量结果 y = ";
        for (int i = 0; i < n_vars; ++i) {
            if (i > 0) cout << (sign[i]==1?" + ":" - ");
            cout << x[i];
        }
        cout << " = " << y << endl;
        cout << "合成标准不确定度 u_y = √(u1?+u2?+...) = " << uy << endl;
        cout << "相对标准不确定度 u_r: " << ur;
        if (SHOW_PERCENT) cout << " (" << ur * 100 << "%)";
        cout << endl;
        cout << "最终表达式: y = " << y << " ± " << uy << endl;

    } else if (model_choice == 3) { // 三角函数模型
        double x = safe_input<double>("输入角度值（弧度）: ");
        double ux = safe_input<double>("输入角度的合成不确定度: ");
        int func_choice = safe_input<int>("选择三角函数:\n1. sin(x)\n2. cos(x)\n3. tan(x)\n输入选项: ");

        double y, uy;
        switch (func_choice) {
            case 1:
                y = sin(x);
                uy = abs(cos(x)) * ux; // 导数：cos(x)
                break;
            case 2:
                y = cos(x);
                uy = abs(sin(x)) * ux; // 导数：-sin(x)
                break;
            case 3:
                y = tan(x);
                uy = abs(1 / cos(x) / cos(x)) * ux; // 导数：sec?(x)
                break;
            default:
                cout << "无效选择，默认计算sin(x)" << endl;
                y = sin(x);
                uy = abs(cos(x)) * ux;
        }

        double ur = uy / abs(y);
        cout << "\n【间接测量最终结果】" << endl;
        cout << fixed << setprecision(PRECISION);
        cout << "y = " << (func_choice==1?"sin":"cos") << "(" << x << ") = " << y << endl;
        cout << "合成标准不确定度 u_y: " << uy << endl;
        cout << "相对标准不确定度 u_r: " << ur;
        if (SHOW_PERCENT) cout << " (" << ur * 100 << "%)";
        cout << endl;
        cout << "最终表达式: y = " << y << " ± " << uy << endl;
    }
}

/**
 * @brief 显示帮助信息
 */
void show_help() {
    cout << "\n===== 使用帮助 =====" << endl;
    cout << "1. 直接测量模块：" << endl;
    cout << "   - A类不确定度：支持手动输入、贝塞尔公式、极差法" << endl;
    cout << "   - B类不确定度：支持手动输入、基于仪器误差自动计算（均匀/正态/三角分布）" << endl;
    cout << "2. 间接测量模块：" << endl;
    cout << "   - 乘除/幂模型：y = k*x1^p1*x2^p2*...（p为负表示除法）" << endl;
    cout << "   - 加减模型：y = x1 ± x2 ± ..." << endl;
    cout << "   - 三角函数模型：y = sin(x)/cos(x)/tan(x)（x为弧度）" << endl;
    cout << "3. 所有输入均做了有效性验证，可放心使用。" << endl;
}

// ===================== 主函数 =====================
int main() {
    cout << "=====================================" << endl;
    cout << "  大学物理实验不确定度计算工具 v2.0  " << endl;
    cout << "  支持：A/B类合成、多次测量、误差传递  " << endl;
    cout << "  扩展：极差法、多变量、三角函数、输入验证  " << endl;
    cout << "=====================================" << endl;

    while (true) {
        cout << "\n请选择功能模块 (输入编号):" << endl;
        cout << "1. 直接测量 (A类 + B类 合成)" << endl;
        cout << "2. 间接测量 (不确定度传递)" << endl;
        cout << "3. 使用帮助" << endl;
        cout << "0. 退出程序" << endl;
        
        int cmd = safe_input<int>("选择: ");
        switch (cmd) {
            case 1: 
                direct_measurement_calc(); 
                break;
            case 2: 
                indirect_measurement_calc(); 
                break;
            case 3:
                show_help();
                break;
            case 0: 
                cout << "\n程序结束，谢谢使用！" << endl;
                return 0;
            default:
                cout << "输入错误，请输入0-3之间的数字！" << endl;
        }

        // 每次计算完成后，等待用户确认再返回主菜单
        cout << "\n按回车键返回主菜单...";
        cin.get();
        clear_input_buffer();
    }

    return 0;
}
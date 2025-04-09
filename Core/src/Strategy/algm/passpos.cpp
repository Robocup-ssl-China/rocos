#include <cmath>
#include <thread>
#include "fmt/core.h"
#include <Eigen/Core>
#include "staticparams.h"
#include "parammanager.h"
#include "algm_registry.h"
#include "VisionTypeDef.h"
#include "GDebugEngine.h"
class PassPosCalculate: public Algm{
public:
    PassPosCalculate() = default;
    void process(DataMap& data) override;
};
REGISTER_ALGM(PassPosCalculate, PassPosCalculate);

using namespace Eigen;

namespace DEF{
    auto zpm = ZSS::ZParamManager::instance();
    const float FLX = zpm->value("field/width",QVariant(9000)).toFloat();
    const float FLY = zpm->value("field/height",QVariant(6000)).toFloat();
    const float PLX = zpm->value("field/penaltyWidth",QVariant(1000)).toFloat();
    const float PLY = zpm->value("field/penaltyLength",QVariant(2000)).toFloat();
    const Vector2f GOAL = {DEF::FLX/2, 0};
    const float GL = zpm->value("field/goalWidth",QVariant(1000)).toFloat();
    constexpr float ROBOT_RADIUS = PARAM::Vehicle::V2::PLAYER_SIZE;
    constexpr float STEP = 150.0;
    constexpr float MAX_ACC = 4000;
    constexpr float MAX_VEL = 3500;
    constexpr float MAX_BALL_VEL = 5500;
    constexpr float MAX_PASS_VEL = 4000;

    constexpr size_t POINTS_MAX_NUM = 4000;
    constexpr size_t SHOOT_SIMULATION_NUM = 6;
}
// 生成网格点的辅助函数
MatrixXf generate_grid_points(
    float x_start, float x_end, float x_step,
    float y_start, float y_end, float y_step) {
    std::vector<Vector2f> points;
    
    // 处理浮点数精度问题
    auto generate_sequence = [](float start, float end, float step) {
        std::vector<float> seq;
        for (float v = start; v <= end + 1e-6; v += step){
            seq.push_back(v);
        }
        return seq;
    };
    
    // 生成坐标序列
    const auto x_values = generate_sequence(x_start, x_end, x_step);
    const auto y_values = generate_sequence(y_start, y_end, y_step);
    
    // 创建网格点
    for (float x : x_values)
        for (float y : y_values)
            points.emplace_back(x, y);
    
    // 转换为Eigen矩阵
    MatrixXf mat(points.size(), 2);
    for (size_t i = 0; i < points.size(); ++i)
        mat.row(i) = points[i];
    
    return mat;
}

std::pair<MatrixXf, VectorXf> get_points_and_sizes(
    const MatrixXf& robots) {
    using namespace Eigen;
    std::vector<Vector2f> points;
    std::vector<float> sizes;
    const float R = DEF::ROBOT_RADIUS;

    // 第一部分：后场点（低分辨率）
    {
        MatrixXf grid = generate_grid_points(
            -DEF::FLX/2 - R, 0, 3.0*DEF::STEP,
            -DEF::FLY/2, DEF::FLY/2, 3.0*DEF::STEP
        );
        for (int i = 0; i < grid.rows(); ++i) {
            points.emplace_back(grid.row(i));
            sizes.push_back(3*DEF::STEP);
        }
    }

    // 第二部分：前场点（中等分辨率）
    {
        MatrixXf grid = generate_grid_points(
            0, DEF::FLX/2, 1.0*DEF::STEP,
            -DEF::FLY/2, DEF::FLY/2, 1.0*DEF::STEP
        );
        for (int i = 0; i < grid.rows(); ++i) {
            points.emplace_back(grid.row(i));
            sizes.push_back(1.1*DEF::STEP);
        }
    }

    // 第三部分：机器人周围点（高分辨率）
    {
        const float dl = DEF::FLX/10;
        MatrixXf grid = generate_grid_points(
            -dl, dl, 0.3*DEF::STEP,
            -dl, dl, 0.3*DEF::STEP
        );
        
        // 过滤圆形区域
        std::vector<Vector2f> filtered;
        for (int i = 0; i < grid.rows(); ++i) {
            if (grid.row(i).norm() < 0.8*dl) { // 添加安全余量
                filtered.emplace_back(grid.row(i));
            }
        }
        
        // 关联到每个机器人（示例代码中暂时注释）
        // for (const auto& r : robots) {
        //     for (const auto& p : filtered) {
        //         points.push_back(p + r);
        //         sizes.push_back(0.3*DEF::STEP);
        //     }
        // }
    }

    // 转换为Eigen对象
    MatrixXf points_mat(points.size(), 2);
    VectorXf sizes_vec(sizes.size());
    for (size_t i = 0; i < points.size(); ++i) {
        points_mat.row(i) = points[i];
        sizes_vec(i) = sizes[i];
    }

    // 创建过滤掩码
    std::vector<bool> valid_mask(points.size(), true);
    for (size_t i = 0; i < points.size(); ++i) {
        const float x = points_mat(i, 0);
        const float y = points_mat(i, 1);
        
        // 定义禁区条件
        const bool in_their_penalty = 
            (x > DEF::FLX/2 - DEF::PLX - R) && 
            (std::abs(y) < DEF::PLY/2 + R);
            
        const bool in_our_penalty = 
            (x < -DEF::FLX/2 + DEF::PLX + R) && 
            (std::abs(y) < DEF::PLY/2 + R);
            
        const bool out_of_field = 
            (std::abs(x) > DEF::FLX/2 - R) || 
            (std::abs(y) > DEF::FLY/2 - R);
            
        valid_mask[i] = !(in_their_penalty || in_our_penalty || out_of_field);
    }

    // 应用过滤
    std::vector<Vector2f> final_points;
    std::vector<float> final_sizes;
    for (size_t i = 0; i < valid_mask.size(); ++i) {
        if (valid_mask[i]) {
            final_points.push_back(points_mat.row(i));
            final_sizes.push_back(sizes_vec(i));
        }
    }

    // 最终转换
    MatrixXf result_points(final_points.size(), 2);
    VectorXf result_sizes(final_sizes.size());
    for (size_t i = 0; i < final_points.size(); ++i) {
        result_points.row(i) = final_points[i];
        result_sizes(i) = final_sizes[i];
    }

    return {result_points, result_sizes};
}

/*
 * @brief 计算多个点到目标点的欧氏距离
 * @param pos Nx2的矩阵，每行表示一个点的坐标
 * @param target 2维列向量，表示目标点坐标
 * @return N维列向量，包含每个点到目标的距离
 */
VectorXf eigen_dist(const MatrixXf& pos, 
                           const Vector2f& target) {
    // 验证输入维度
    if(pos.cols() != 2 || target.size() != 2) {
        throw std::invalid_argument("Invalid input dimensions");
    }

    // 使用Eigen的广播机制计算差值
    const MatrixXf diff = pos.rowwise() - target.transpose();
    
    // 计算每行的L2范数
    return diff.rowwise().norm();
}

MatrixXf eigen_dist_matrix(const MatrixXf& A,
                                  const MatrixXf& B) {
    // 验证维度一致性
    assert(A.cols() == B.cols());
    
    // 计算各点的平方范数
    const VectorXf A_sq_norm = A.rowwise().squaredNorm();
    const RowVectorXf B_sq_norm = B.rowwise().squaredNorm().transpose();
    
    // 展开距离平方公式：||a-b||² = ||a||² + ||b||² - 2a·b
    MatrixXf dist_sq = 
        A_sq_norm * RowVectorXf::Ones(B.rows()) +  // 扩展A的范数
        VectorXf::Ones(A.rows()) * B_sq_norm -    // 扩展B的范数
        2 * (A * B.transpose());                          // 点积项
    
    // 处理数值误差并开方
    return dist_sq.cwiseMax(0.0).cwiseSqrt();            // 确保非负
}

// VectorXf calc_interception(const MatrixXf& points,
//                                   const Vector2f& ball,
//                                   const MatrixXf& robot,
//                                   const MatrixXf& enemy,
//                                   float shoot_speed = DEF::MAX_BALL_VEL){
//     MatrixXf lines = points.rowwise() - ball.transpose();
//     MatrixXf enemy_relative = enemy.rowwise() - ball.transpose();
//     VectorXf angles = lines.rowwise().norm().binaryExpr(lines.colwise().sum(),
//         [](float a, float b) { return std::atan2(a, b); });

//     return VectorXf::Zero(points.rows());
// }

void PassPosCalculate::process(DataMap& data){
    auto vision = data.get_as<VisualInfoT>("vision");
    auto& info_our_player = vision.player[VisualInfoT::ME];
    auto& info_oppo_player = vision.player[VisualInfoT::OPPONENT];
    auto& info_ball = vision.ball;

    MatrixXf robot;
    MatrixXf enemy;
    Vector2f ball = {info_ball.rawPos.x, info_ball.rawPos.y};

    {// 转换信息到MatrixXf
        std::vector<Vector2f> robot_vector;
        std::vector<Vector2f> enemy_vector;
        for(int i = 0; i < PARAM::Field::MAX_PLAYER; ++i){
            if(info_our_player[i].valid){
                robot_vector.emplace_back(info_our_player[i].pos.x, info_our_player[i].pos.y);
            }
            if(info_oppo_player[i].valid){
                enemy_vector.emplace_back(info_oppo_player[i].pos.x, info_oppo_player[i].pos.y);
            }
        }
        robot.resize(robot_vector.size(), 2);
        enemy.resize(enemy_vector.size(), 2);
        for(int i = 0; i < robot_vector.size(); ++i){
            robot.row(i) = robot_vector[i];
        }
        for(int i = 0; i < enemy_vector.size(); ++i){
            enemy.row(i) = enemy_vector[i];
        }
    }


    auto [points, sizes] = get_points_and_sizes(robot);
    const size_t num_points = points.rows();
    VectorXf value = VectorXf::Zero(num_points);

    {// near to goal
        constexpr float min_clip = 2000.0;
        constexpr float max_clip = 5000.0;
        constexpr float scale = 3000.0;
        VectorXf distances = eigen_dist(points, DEF::GOAL);
        VectorXf clipped = distances.cwiseMax(min_clip).cwiseMin(max_clip);
        VectorXf x = (-clipped.array()/scale).matrix();
        value += 1.0 * x;
    }{// near to robot
        constexpr float scale = 3000.0;
        constexpr float min_clip = 0.3;
        constexpr float max_clip = 1.0;
        if (robot.rows() > 0) {
            MatrixXf point_robot_dist = eigen_dist_matrix(points, robot) / scale;
            fmt::print("point_robot_dist.size: {},{}\n", point_robot_dist.rows(), point_robot_dist.cols());
            VectorXf min_dist = point_robot_dist.rowwise().minCoeff();
            VectorXf x = -(min_dist.cwiseMax(min_clip).cwiseMin(max_clip));
            fmt::print("x.size: {},{}\n", x.rows(), x.cols());
            value += 1.0 * x;
        }
    }{// far from enemy
        constexpr float scale = 3000.0;
        constexpr float min_clip = 0.0;
        constexpr float max_clip = 0.3;
        if (enemy.rows() > 0) {
            MatrixXf point_enemy_dist = eigen_dist_matrix(points, enemy) / scale;
            VectorXf min_dist = point_enemy_dist.rowwise().minCoeff();
            VectorXf x = -(min_dist.cwiseMax(min_clip).cwiseMin(max_clip));
            value += 1.0 * x;
        }
    }{// dist to ball 
        constexpr float scale = 2000.0;
        constexpr float min_clip = 0.2;
        constexpr float max_clip = 1.0;
        VectorXf dist_ball = eigen_dist(points, ball) / scale;
        VectorXf x = -1.0 * (dist_ball.cwiseMax(min_clip).cwiseMin(max_clip).cwiseInverse());
        value += 1.0 * x;
    }{// intercept by enemy
        // calc_interception(points, ball, robot, enemy);
    }{// shoot angle

    }{// available shoot simulation

    }


    fmt::print("points size: {}\n", num_points);
    {
        // normalize value to [0,1]
        VectorXf norm_value = (value.array() - value.minCoeff()) / (value.maxCoeff() - value.minCoeff());
        std::vector<float> _x;
        std::vector<float> _y;
        std::vector<float> _value;
        for(int i = 0; i < std::min(num_points, DEF::POINTS_MAX_NUM); ++i){
            auto point = points.row(i);
            _x.push_back(point(0));
            _y.push_back(point(1));
            _value.push_back(norm_value(i));
        }
        GHeatmapEngine::instance()->gui_debug_heat(_x, _y, _value, DEF::STEP);
    }
}